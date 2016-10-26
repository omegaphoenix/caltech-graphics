#include "draw_pixels.hpp"

#include <algorithm>
#include <cstddef>
#include <float.h>
#include <iostream>
#include <math.h> // round
#include <memory> // shared_ptr
#include <vector>

#include "camera.hpp"
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
using NormalPtr = shared_ptr<Normal>;
using ReflectPtr = shared_ptr<struct Reflectance>;
using ModelPtr = shared_ptr<Model>;

using LightVecPtr = shared_ptr<vector<Light>>;
using ModelVectorPtr = shared_ptr<vector<ModelPtr>>;

void output_ppm(int xres, int yres, Pixel **grid) {
  start_ppm_output(xres, yres);
  for (int y = 0; y < yres; y++) {
    for (int x = 0; x < xres; x++) {
      output_pixel(y, x, grid);
    }
  }
}

void start_ppm_output(int xres, int yres) {
  cout << "P3" << endl;
  cout << xres << " " << yres << endl;
  cout << MAX_INTENSITY << endl;
}

void output_pixel(int row, int col, Pixel **grid) {
  Pixel pix = grid[row][col];
  int red = (int)(round(pix.red * MAX_INTENSITY));
  int green = (int)(round(pix.green * MAX_INTENSITY));
  int blue = (int)(round(pix.blue * MAX_INTENSITY));
  cout << red << " " << green << " " << blue << endl;
}

vector<Vertex> NDCs_to_pixels(int xres, int yres, vector<Vertex> ndc_vertices) {
    vector<Vertex> vertices = vector<Vertex>();
    // Initial vertex should be filler
    vertices.push_back(Vertex());

    vector<Vertex>::iterator vertex_it = ++(ndc_vertices.begin());
    while (vertex_it != ndc_vertices.end()) {
      vertices.push_back(NDC_to_pixel(xres, yres, *vertex_it));
      ++vertex_it;
    }

    return vertices;
}

Vertex NDC_to_pixel(int xres, int yres, Vertex ndc_vertex) {
  int new_x = round((ndc_vertex.x + 1)*xres/2);
  int new_y = round((ndc_vertex.y - 1)*yres/-2);
  return Vertex(new_x, new_y, ndc_vertex.z);
}

void rasterize(Vertex v1, Vertex v2, Pixel **grid, int xres, int yres) {
  if (is_on_screen(v1, xres, yres) && is_on_screen(v2, xres, yres)) {
    bresenham(v1.x, v1.y, v2.x, v2.y, grid);
  }
}

bool is_on_screen(Vertex v, int xres, int yres) {
  return (v.x > 0) && (v.x < xres) && (v.y > 0) && (v.y < yres);
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
    fill(x, y, grid);
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
    fill(x, y, grid);
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
    fill(x, y, grid);
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
    fill(x, y, grid);
    if (((epsilon + dy) << 1) > -1*dx) {
      epsilon = epsilon + dy;
    } else {
      epsilon = epsilon + dy + dx;
      y = y - 1;
    }
  }
}

void fill(int x, int y, Pixel **grid) {
  grid[y][x] = Pixel(MAX_INTENSITY, MAX_INTENSITY, MAX_INTENSITY);
}

void fill(int x, int y, Pixel **grid, Pixel color) {
  grid[y][x] = color;
}
