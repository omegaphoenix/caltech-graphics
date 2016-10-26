#include "shading.hpp"

#include <algorithm>
#include <cstddef>
#include <float.h>
#include <iostream>
#include <math.h> // round
#include <memory> // shared_ptr
#include <vector>

#include "camera.hpp"
#include "draw_pixels.hpp"
#include "face.hpp"
#include "light.hpp"
#include "normal.hpp"
#include "model.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

const int MAX_INTENSITY = 255;

using namespace std;

using CameraPtr = shared_ptr<Camera>;
using FacePtr = shared_ptr<Face>;
using LightPtr = shared_ptr<Light>;
using MaterialPtr = shared_ptr<Material>;
using MatrixPtr = shared_ptr<Eigen::MatrixXd>;
using ReflectPtr = shared_ptr<struct Reflectance>;

using LightVecPtr = shared_ptr<vector<Light>>;
using ModelVectorPtr = shared_ptr<vector<ModelPtr>>;

void phong(vector<Model> models, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid) {
  double **buffer = new_buffer(xres, yres); // for depth buffering
  for (vector<Model>::iterator model_it = models.begin(); model_it != models.end(); ++model_it) {
    phong_faces(*model_it, lights, cam, xres, yres, grid, buffer);
  }

  delete_buffer(xres, yres, buffer);
  delete[] buffer;
}

void gouraud(vector<Model> models, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid) {
  double **buffer = new_buffer(xres, yres); // for depth buffering
  for (vector<Model>::iterator model_it = models.begin(); model_it != models.end(); ++model_it) {
    gouraud_faces(*model_it, lights, cam, xres, yres, grid, buffer);
  }

  delete_buffer(xres, yres, buffer);
  delete[] buffer;
}

void phong_faces(Model model, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer) {
  for (vector<FacePtr>::iterator face_it = model.faces->begin(); face_it != model.faces->end(); ++face_it) {
    FacePtr face = *face_it;
    phong_shading(model, face, model.material, lights, cam, xres, yres, grid, buffer);
  }
}

void phong_shading(Model model, FacePtr face, MaterialPtr material, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer) {
  vector<Vertex> vertices = model.vertex_buffer;
  vector<Normal> normals = model.normals;

  Vertex v_a = vertices[face->vertex1];
  Vertex v_b = vertices[face->vertex2];
  Vertex v_c = vertices[face->vertex3];

  Normal n_a = normals[face->normal1];
  Normal n_b = normals[face->normal2];
  Normal n_c = normals[face->normal3];

  // Transform to NDC coordinates
  Vertex NDC_a = cam->cam_transform(v_a);
  Vertex NDC_b = cam->cam_transform(v_b);
  Vertex NDC_c = cam->cam_transform(v_c);

  // backface culling
  if (backfacing(NDC_a, NDC_b, NDC_c)) return;

  Vertex screen_a = NDC_to_pixel(xres, yres, NDC_a);
  Vertex screen_b = NDC_to_pixel(xres, yres, NDC_b);
  Vertex screen_c = NDC_to_pixel(xres, yres, NDC_c);

  int min_x = min_x_coord(screen_a, screen_b, screen_c);
  int max_x = max_x_coord(screen_a, screen_b, screen_c, xres);
  int min_y = min_y_coord(screen_a, screen_b, screen_c);
  int max_y = max_y_coord(screen_a, screen_b, screen_c, yres);

  // Iterate through rectangular screen coordinates that triangle is in
  for (int x = min_x; x <= max_x; x++) {
    for (int y = min_y; y <= max_y; y++) {
      double alpha = compute_alpha(screen_a, screen_b, screen_c, x, y);
      double beta = compute_beta(screen_a, screen_b, screen_c, x, y);
      double gamma = compute_gamma(screen_a, screen_b, screen_c, x, y);

      if (valid_alpha_beta_gamma(alpha, beta, gamma)) {
        Vertex NDC = combine_vertices(alpha, beta, gamma, NDC_a, NDC_b, NDC_c);

        if (inside_NDC_cube(NDC) && (NDC.z <= buffer[y][x])) {
          // update buffer
          buffer[y][x] = NDC.z;

          Vertex v = combine_vertices(alpha, beta, gamma, v_a, v_b, v_c);
          Normal n = combine_normals(alpha, beta, gamma, n_a, n_b, n_c);

          // Calculate lighting per pixel
          Pixel color = lighting(v, n, material, lights, cam);
          fill(x, y, grid, color);
        }
      }
    }
  }
}

void gouraud_faces(Model model, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer) {
  for (vector<FacePtr>::iterator face_it = model.faces->begin(); face_it != model.faces->end(); ++face_it) {
    FacePtr face = *face_it;
    gouraud_shading(model, face, lights, cam, xres, yres, grid, buffer);
  }
}

void gouraud_shading(Model model, FacePtr face, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer) {
  vector<Vertex> vertices = model.vertex_buffer;
  vector<Normal> normals = model.normals;

  Vertex v_a = vertices[face->vertex1];
  Vertex v_b = vertices[face->vertex2];
  Vertex v_c = vertices[face->vertex3];

  // Calculate lighting each each vertex of face
  Pixel color_a = lighting(v_a, normals[face->normal1], model.material, lights, cam);
  Pixel color_b = lighting(v_b, normals[face->normal2], model.material, lights, cam);
  Pixel color_c = lighting(v_c, normals[face->normal3], model.material, lights, cam);

  // Transform to NDC coordinates
  Vertex NDC_a = cam->cam_transform(v_a);
  Vertex NDC_b = cam->cam_transform(v_b);
  Vertex NDC_c = cam->cam_transform(v_c);

  ColorVertex a = ColorVertex(NDC_a, color_a);
  ColorVertex b = ColorVertex(NDC_b, color_b);
  ColorVertex c = ColorVertex(NDC_c, color_c);

  // Rasterize face triangle
  raster_tri(a, b, c, xres, yres, grid, buffer);
}

Normal combine_normals(double alpha, double beta, double gamma, Normal n_a, Normal n_b, Normal n_c) {
  double x = alpha*n_a.x + beta*n_b.x + gamma*n_c.x;
  double y = alpha*n_a.y + beta*n_b.y + gamma*n_c.y;
  double z = alpha*n_a.z + beta*n_b.z + gamma*n_c.z;

  return Normal(x, y, z);
}

Vertex combine_vertices(double alpha, double beta, double gamma, Vertex v_a, Vertex v_b, Vertex v_c) {
  double x = alpha*v_a.x + beta*v_b.x + gamma*v_c.x;
  double y = alpha*v_a.y + beta*v_b.y + gamma*v_c.y;
  double z = alpha*v_a.z + beta*v_b.z + gamma*v_c.z;

  return Vertex(x, y, z);
}

void raster_tri(ColorVertex NDC_a, ColorVertex NDC_b, ColorVertex NDC_c, int xres, int yres, Pixel **grid, double **buffer) {
  // backface culling
  if (backfacing(NDC_a.ver, NDC_b.ver, NDC_c.ver)) return;

  Vertex a = NDC_to_pixel(xres, yres, NDC_a.ver);
  Vertex b = NDC_to_pixel(xres, yres, NDC_b.ver);
  Vertex c = NDC_to_pixel(xres, yres, NDC_c.ver);

  int min_x = min_x_coord(a, b, c);
  int max_x = max_x_coord(a, b, c, xres);
  int min_y = min_y_coord(a, b, c);
  int max_y = max_y_coord(a, b, c, yres);

  // Iterate through rectangular screen coordinates that triangle is in
  for (int x = min_x; x <= max_x; x++) {
    for (int y = min_y; y <= max_y; y++) {
      double alpha = compute_alpha(a, b, c, x, y);
      double beta = compute_beta(a, b, c, x, y);
      double gamma = compute_gamma(a, b, c, x, y);

      if (valid_alpha_beta_gamma(alpha, beta, gamma)) {
        Vertex NDC = combine_vertices(alpha, beta, gamma, NDC_a.ver, NDC_b.ver, NDC_c.ver);
        if (inside_NDC_cube(NDC) && (NDC.z <= buffer[y][x])) {
          // update buffer
          buffer[y][x] = NDC.z;

          // Combine colors based on proximity to each vertex of face
          double red = alpha * NDC_a.col.red + beta * NDC_b.col.red + gamma * NDC_c.col.red;
          double green = alpha * NDC_a.col.green + beta * NDC_b.col.green + gamma * NDC_c.col.green;
          double blue = alpha * NDC_a.col.blue + beta * NDC_b.col.blue + gamma * NDC_c.col.blue;

          Pixel color = Pixel(red, green, blue);
          fill(x, y, grid, color);
        }
      }
    }
  }
}

Pixel lighting(Vertex v, Normal n, MaterialPtr material, vector<Light> lights, CameraPtr cam) {
  // 3D vectors
  Eigen::MatrixXd c_d(1, 3), c_a(1, 3), c_s(1, 3), diffuse_sum(1, 3), specular_sum(1, 3), e_dir(1, 3);
  Eigen::MatrixXd l_p(1, 3), l_c(1, 3), l_dir(1, 3), l_diffuse(1, 3), l_specular(1, 3);

  // Reflectance to 3D vectors
  c_d = ref_to_mat(material->diffuse);
  c_a = ref_to_mat(material->ambient);
  c_s = ref_to_mat(material->specular);
  double p = material->shininess;

  diffuse_sum << 0, 0, 0;
  specular_sum << 0, 0, 0;

  e_dir = normalize_vec(ver_to_mat(cam->pos) - ver_to_mat(v));

  for (vector<Light>::iterator light_it = lights.begin(); light_it != lights.end(); ++light_it) {
    Light light = *light_it;

    float x = light.position[0]/light.position[3];
    float y = light.position[1]/light.position[3];
    float z = light.position[2]/light.position[3];

    float r = light.color[0];
    float g = light.color[1];
    float b = light.color[2];

    l_p << x, y, z;
    l_c << r, g, b;
    double dist = magnitude(l_p - ver_to_mat(v));

    // Multiply by attenuation factor
    l_c = l_c / (1 + (*light_it).attenuation_k * dist*dist);
    l_dir = normalize_vec(l_p - ver_to_mat(v));

    l_diffuse = l_c * (max(0.0, (norm_to_mat(n) * l_dir.transpose()).sum()));
    diffuse_sum = diffuse_sum + l_diffuse;

    l_specular = l_c * pow((max(0.0, (norm_to_mat(n) * (normalize_vec(e_dir + l_dir)).transpose()).sum())),p);
    specular_sum = specular_sum + l_specular;
  }

  double red = min(1.0, c_a(0, 0) + diffuse_sum(0, 0)*c_d(0, 0) + specular_sum(0, 0)*c_s(0, 0));
  double green = min(1.0, c_a(0, 1) + diffuse_sum(0, 1)*c_d(0, 1) + specular_sum(0, 1)*c_s(0, 1));
  double blue = min(1.0, c_a(0, 2) + diffuse_sum(0, 2)*c_d(0, 2) + specular_sum(0, 2)*c_s(0, 2));

  return Pixel(red, green, blue);
}

// Backfacing if z-coordinate of cross product of (c-b) x (a-b) is negative
bool backfacing(Vertex NDC_a, Vertex NDC_b, Vertex NDC_c) {
  // Cross product of (c-b) x (a-b)
  Eigen::MatrixXd cross =
    cross_product_vec(ver_to_mat(NDC_c) - ver_to_mat(NDC_b),
      ver_to_mat(NDC_a) - ver_to_mat(NDC_b)) ;

  // Return true if z coordinate of cross product is negative
  return cross(0,2) < 0;
}

bool inside_NDC_cube(Vertex NDC) {
  return in_range(NDC.x, -1, 1) && in_range(NDC.y, -1, 1) && in_range(NDC.z, -1, 1);
}

bool valid_alpha_beta_gamma(double alpha, double beta, double gamma) {
  return in_range(alpha, 0, 1) && in_range(beta, 0, 1) && in_range(gamma, 0, 1);
}

bool in_range(double val, double low, double high) {
  return low <= val && high >= val;
}

double compute_alpha(Vertex a, Vertex b, Vertex c, int x, int y) {
  return f_ij(b, c, x, y) / f_ij(b, c, a.x, a.y);
}

double compute_beta(Vertex a, Vertex b, Vertex c, int x, int y) {
  return f_ij(a, c, x, y) / f_ij(a, c, b.x, b.y);
}

double compute_gamma(Vertex a, Vertex b, Vertex c, int x, int y) {
  return f_ij(a, b, x, y) / f_ij(a, b, c.x, c.y);
}

double f_ij(Vertex i, Vertex j, double x, double y) {
  return (i.y - j.y)*x + (j.x - i.x)*y + i.x*j.y - j.x*i.y;
}

int min_x_coord(Vertex a, Vertex b, Vertex c) {
  return max(0, (int)min(a.x, min(b.x, c.x)));
}

int max_x_coord(Vertex a, Vertex b, Vertex c, int xres) {
  return min(xres-1, (int)max(a.x, max(b.x, c.x)));
}

int min_y_coord(Vertex a, Vertex b, Vertex c) {
  return max(0, (int)min(a.y, min(b.y, c.y)));
}

int max_y_coord(Vertex a, Vertex b, Vertex c, int yres) {
  return min(yres-1, (int)max(a.y, max(b.y, c.y)));
}

Eigen::MatrixXd ref_to_mat(ReflectPtr reflect) {
  Eigen::MatrixXd res(1, 3);
  res << reflect->red, reflect->green, reflect->blue;
  return res;
}

Eigen::MatrixXd ver_to_mat(Vertex ver) {
  Eigen::MatrixXd res(1, 3);
  res << ver.x, ver.y, ver.z;
  return res;
}

Eigen::MatrixXd norm_to_mat(Normal norm) {
  Eigen::MatrixXd res(1, 3);
  res << norm.x, norm.y, norm.z;
  return res;
}

Eigen::MatrixXd normalize_vec(Eigen::MatrixXd mat) {
  double x = mat(0, 0);
  double y = mat(0, 1);
  double z = mat(0, 2);
  double magnitude = sqrt(x*x + y*y + z*z);
  return mat/magnitude;
}

Eigen::MatrixXd cross_product_vec(Eigen::MatrixXd vec_u, Eigen::MatrixXd vec_v) {
  Eigen::MatrixXd res(1, 3);
  double i_val = vec_u(0, 1)*vec_v(0, 2) - vec_u(0, 2)*vec_v(0, 1);
  double j_val = vec_u(0, 2)*vec_v(0, 0) - vec_u(0, 0)*vec_v(0, 2);
  double k_val = vec_u(0, 0)*vec_v(0, 1) - vec_u(0, 1)*vec_v(0, 0);
  res << i_val, j_val, k_val;
  return res;
}

double magnitude(Eigen::MatrixXd diff_mat) {
  double x = diff_mat(0, 0);
  double y = diff_mat(0, 1);
  double z = diff_mat(0, 2);
  return sqrt(x*x + y*y + z*z);
}

double **new_buffer(int xres, int yres) {
  double **buffer = new double *[yres];
  for (int y = 0; y < yres; y++) {
    buffer[y] = new double[xres];
    for (int x = 0; x < xres; x++) {
      buffer[y][x] = DBL_MAX;
    }
  }
  return buffer;
}

void delete_buffer(int xres, int yres, double **buffer) {
  for (int y = 0; y < yres; y++) {
    delete[] buffer[y];
  }
}
