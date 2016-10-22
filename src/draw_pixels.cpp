#include "draw_pixels.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <math.h> // round
#include <memory> // shared_ptr
#include <vector>

#include "camera.hpp"
#include "light.hpp"
#include "normal.hpp"
#include "three_d_model.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

using CameraPtr = shared_ptr<Camera>;
using LightPtr = shared_ptr<Light>;
using MaterialPtr = shared_ptr<Material>;
using MatrixPtr = shared_ptr<Eigen::MatrixXd>;
using NormalPtr = shared_ptr<Normal>;
using ReflectPtr = shared_ptr<struct Reflectance>;
using VertexPtr = shared_ptr<Vertex>;

using LightVecPtr = shared_ptr<vector<LightPtr>>;
using VerVectorPtr = shared_ptr<vector<VertexPtr>>;

void raster_tri(ColorVertex NDC_a, ColorVertex NDC_b, ColorVertex NDC_c, int xres, int yres, Pixel **grid, double **buffer) {
  Eigen::MatrixXd cross = cross_product_vec(ver_to_mat(NDC_c.ver) - ver_to_mat(NDC_b.ver),
      ver_to_mat(NDC_a.ver) - ver_to_mat(NDC_b.ver)) ;

  if (cross(0, 2) < 0) {
    return;
  }

  VertexPtr a = NDC_to_pixel(xres, yres, NDC_a.ver);
  VertexPtr b = NDC_to_pixel(xres, yres, NDC_b.ver);
  VertexPtr c = NDC_to_pixel(xres, yres, NDC_c.ver);

  int min_x = min_x_coord(a, b, c);
  int max_x = max_x_coord(a, b, c);
  int min_y = min_y_coord(a, b, c);
  int max_y = min_y_coord(a, b, c);

  for (int x = min_x; x <= max_x; x++) {
    for (int y = min_y; y <= max_y; y++) {
      double alpha = compute_alpha(a, b, c, x, y);
      double beta = compute_beta(a, b, c, x, y);
      double gamma = compute_gamma(a, b, c, x, y);

      if (valid_alpha_beta_gamma(alpha, beta, gamma)) {
        VertexPtr NDC = create_NDC_point(alpha, beta, gamma, NDC_a.ver, NDC_b.ver, NDC_c.ver);
        if (inside_NDC_cube(alpha, beta, gamma, NDC_a.ver, NDC_b.ver, NDC_c.ver)
            && !(NDC->z > buffer[y][x])) {
          buffer[y][x] = NDC->z;

          int red = round(alpha * NDC_a.col.red + beta * NDC_b.col.red + gamma * NDC_b.col.red);
          int green = round(alpha * NDC_a.col.green + beta * NDC_b.col.green + gamma * NDC_b.col.green);
          int blue = round(alpha * NDC_a.col.blue + beta * NDC_b.col.blue + gamma * NDC_b.col.blue);

          Pixel color = Pixel(red, green, blue);
          fill(x, y, grid, color);
        }
      }
    }
  }
}

Eigen::MatrixXd cross_product_vec(Eigen::MatrixXd vec_u, Eigen::MatrixXd vec_v) {
  Eigen::MatrixXd res;
  double i_val = vec_u(0,1)*vec_v(0,2) - vec_u(0,2)*vec_v(0,1);
  double j_val = vec_u(0,2)*vec_v(0,0) - vec_u(0,0)*vec_v(0,2);
  double k_val = vec_u(0,0)*vec_v(0,1) - vec_u(0,0)*vec_v(0,1);
  res << i_val, j_val, k_val;
}

VertexPtr create_NDC_point(double alpha, double beta, double gamma, VertexPtr a, VertexPtr b, VertexPtr c) {
  double x = alpha*a->x + beta*b->x + gamma*c->x;
  double y = alpha*a->y + beta*b->y + gamma*c->y;
  double z = alpha*a->z + beta*b->z + gamma*c->z;
  return VertexPtr(new Vertex(x, y, z));
}

bool inside_NDC_cube(double alpha, double beta, double gamma, VertexPtr a, VertexPtr b, VertexPtr c) {
  return in_range(alpha*a->x, -1, 1) && in_range(beta*b->x, -1, 1) && in_range(gamma*c->x, -1, 1)
    && in_range(alpha*a->y, -1, 1) && in_range(beta*b->y, -1, 1) && in_range(gamma*c->y, -1, 1);
}

bool valid_alpha_beta_gamma(double alpha, double beta, double gamma) {
  return in_range(alpha, 0, 1) && in_range(beta, 0, 1) && in_range(gamma, 0, 1);
}

bool in_range(double val, double low, double high) {
  return low <= val && high >= val;
}

double compute_alpha(VertexPtr a, VertexPtr b, VertexPtr c, int x, int y) {
  return f_ij(b, c, x, y) / f_ij(b, c, a->x, a->y);
}

double compute_beta(VertexPtr a, VertexPtr b, VertexPtr c, int x, int y) {
  return f_ij(a, c, x, y) / f_ij(a, c, b->x, b->y);
}

double compute_gamma(VertexPtr a, VertexPtr b, VertexPtr c, int x, int y) {
  return f_ij(a, b, x, y) / f_ij(a, b, c->x, c->y);
}

int min_x_coord(VertexPtr a, VertexPtr b, VertexPtr c) {
  return (int)min(a->x, min(b->x, c->x));
}

int max_x_coord(VertexPtr a, VertexPtr b, VertexPtr c) {
  return (int)max(a->x, max(b->x, c->x));
}

int min_y_coord(VertexPtr a, VertexPtr b, VertexPtr c) {
  return (int)min(a->y, min(b->y, c->y));
}

int max_y_coord(VertexPtr a, VertexPtr b, VertexPtr c) {
  return (int)max(a->y, max(b->y, c->y));
}

double f_ij(VertexPtr i, VertexPtr j, double x, double y) {
  return (i->y - j->y)*x + (j->x - i->x)*y + i->x*j->y - j->x*i->y;
}

Pixel lighting(VertexPtr v, NormalPtr n, MaterialPtr material, LightVecPtr lights, CameraPtr cam) {
  Eigen::MatrixXd c_d, c_a, c_s, diffuse_sum, specular_sum, e_dir;
  Eigen::MatrixXd l_p, l_c, l_dir, l_diffuse, l_specular;

  c_d = ref_to_mat(material->diffuse);
  c_a = ref_to_mat(material->ambient);
  c_s = ref_to_mat(material->specular);
  double p = material->shininess;

  diffuse_sum << 0, 0, 0;
  specular_sum << 0, 0, 0;
  e_dir = normalize_vec(ver_to_mat(cam->pos) - ver_to_mat(v));

  for (vector<LightPtr>::iterator light_it = lights->begin(); light_it != lights->end(); ++light_it) {
    l_p << (*light_it)->x, (*light_it)->y, (*light_it)->z;
    l_c << (*light_it)->r, (*light_it)->g, (*light_it)->b;
    double dist = vec_distance(l_p - ver_to_mat(v));
    l_c = l_c / (1 + (*light_it)->attenuation * dist*dist);
    l_dir = normalize_vec(l_p - ver_to_mat(v));

    l_diffuse = l_c * (max(0, (int)(norm_to_mat(n) * l_dir.transpose()).sum()));
    diffuse_sum = diffuse_sum + l_diffuse;

    l_specular = l_c * pow((max(0, (int)(round((norm_to_mat(n) * normalize_vec(e_dir + l_dir)).sum())))),p);
    specular_sum = specular_sum + l_specular;
  }

  int red = min(1, (int)(round(c_a(0,0) + diffuse_sum(0,0)*c_d(0,0) + specular_sum(0,0)*c_s(0,0))));
  int green = min(1, (int)(round(c_a(0,1) + diffuse_sum(0,1)*c_d(0,1) + specular_sum(0,1)*c_s(0,1))));
  int blue = min(1, (int)(round(c_a(0,2) + diffuse_sum(0,2)*c_d(0,2) + specular_sum(0,2)*c_s(0,2))));

  return Pixel(red, green, blue);
}

double vec_distance(Eigen::MatrixXd diff_mat) {
  double x = diff_mat(0,0);
  double y = diff_mat(0,1);
  double z = diff_mat(0,2);
  return sqrt(x*x + y*y + z*z);
}

Eigen::MatrixXd ref_to_mat(ReflectPtr reflect) {
  Eigen::MatrixXd res;
  res << reflect->red, reflect->green, reflect->blue;
  return res;
}

Eigen::MatrixXd ver_to_mat(VertexPtr ver) {
  Eigen::MatrixXd res;
  res << ver->x, ver->y, ver->z;
  return res;
}

Eigen::MatrixXd norm_to_mat(NormalPtr norm) {
  Eigen::MatrixXd res;
  res << norm->x, norm->y, norm->z;
  return res;
}

Eigen::MatrixXd normalize_vec(Eigen::MatrixXd mat) {
  double sum = mat.sum();
  return mat/sum;
}

void output_ppm(int xres, int yres, Pixel **grid) {
  start_ppm_output(xres, yres);
  for (int y = 0; y < yres; y++) {
    for (int x = 0; x < xres; x++) {
      output_pixel(x, y, grid);
    }
  }
}

void start_ppm_output(int xres, int yres) {
  cout << "P3" << endl;
  cout << xres << " " << yres << endl;
  cout << "255" << endl;
}

void output_pixel(int row, int col, Pixel **grid) {
}

void purple() {
  cout << "85 37 130" << endl;
}

void gold() {
  cout << "253 185 39" << endl;
}

VerVectorPtr NDCs_to_pixels(int xres, int yres, VerVectorPtr ndc_vertices) {
    VerVectorPtr vertices =
      VerVectorPtr(new vector<VertexPtr>());
    vertices->push_back(NULL);

    vector<VertexPtr>::iterator vertex_it = ++(ndc_vertices->begin());
    while (vertex_it != ndc_vertices->end()) {
      vertices->push_back(NDC_to_pixel(xres, yres, *vertex_it));
      ++vertex_it;
    }

    return vertices;
}

VertexPtr NDC_to_pixel(int xres, int yres, VertexPtr ndc_vertex) {
  int new_x = round((ndc_vertex->x + 1)*xres/2);
  int new_y = round((ndc_vertex->y - 1)*yres/-2);
  return VertexPtr(new Vertex(new_x, new_y, ndc_vertex->z));
}

void rasterize(VertexPtr v1, VertexPtr v2, Pixel **grid, int xres, int yres) {
  if (is_on_screen(v1, xres, yres) && is_on_screen(v2, xres, yres)) {
    bresenham(v1->x, v1->y, v2->x, v2->y, grid);
  }
}

bool is_on_screen(VertexPtr v, int xres, int yres) {
  return (v->x > 0) && (v->x < xres) && (v->y > 0) && (v->y < yres);
}

void bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid) {
  int dx = x_1 - x_0;
  int dy = y_1 - y_0;

  if (x_0 > x_1) {
    // Swap points so we only have to consider 4 octants
    bresenham(x_1, y_1, x_0, y_0, grid);
  } else {
    if (dy >= 0 && dx >= dy) {
      first_octant_bresenham(x_0, y_0, x_1, y_1, grid);
    } else if (dy >= dx) {
      second_octant_bresenham(x_0, y_0, x_1, y_1, grid);
    } else if (-1*dy >= dx) {
      seventh_octant_bresenham(x_0, y_0, x_1, y_1, grid);
    } else if (dy < 0 && dx >= -1*dy) {
      eighth_octant_bresenham(x_0, y_0, x_1, y_1, grid);
    } else {
      throw "No octant found";
    }
  }
}

void first_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid) {
  int epsilon = 0;
  int y = y_0;
  int dx = x_1 - x_0;
  int dy = y_1 - y_0;

  for (int x = x_0; x <= x_1; x++) {
    // fill(x, y, grid);
    if (((epsilon + dy) << 1) < dx) {
      epsilon = epsilon + dy;
    } else {
      epsilon = epsilon + dy - dx;
      y = y + 1;
    }
  }
}

void second_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid) {
  int epsilon = 0;
  int x = x_0;
  int dx = x_1 - x_0;
  int dy = y_1 - y_0;

  for (int y = y_0; y <= y_1; y++) {
    // fill(x, y, grid);
    if (((epsilon + dx) << 1) < dy) {
      epsilon = epsilon + dx;
    } else {
      epsilon = epsilon + dx - dy;
      x = x + 1;
    }
  }
}

void seventh_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid) {
  int epsilon = 0;
  int x = x_0;
  int dx = x_1 - x_0;
  int dy = y_1 - y_0;

  for (int y = y_0; y >= y_1; y--) {
    // fill(x, y, grid);
    if (((epsilon + dx) << 1) < -1*dy) {
      epsilon = epsilon + dx;
    } else {
      epsilon = epsilon + dx + dy;
      x = x + 1;
    }
  }
}

void eighth_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid) {
  int epsilon = 0;
  int y = y_0;
  int dx = x_1 - x_0;
  int dy = y_1 - y_0;

  for (int x = x_0; x <= x_1; x++) {
    // fill(x, y, grid);
    if (((epsilon + dy) << 1) > -1*dx) {
      epsilon = epsilon + dy;
    } else {
      epsilon = epsilon + dy + dx;
      y = y - 1;
    }
  }
}

void fill(int x, int y, Pixel **grid, Pixel color) {
  grid[y][x] = color;
}
