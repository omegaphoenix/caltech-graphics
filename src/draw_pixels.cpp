#include "draw_pixels.hpp"

#include <cstddef>
#include <iostream>
#include <math.h> // round
#include <memory> // shared_ptr
#include <vector>

#include "vertex.hpp"

using namespace std;
using VertexPtr = shared_ptr<Vertex>;
using VerVectorPtr = shared_ptr<vector<VertexPtr>>;

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
  if (grid[col][row].colored) {
    purple();
  } else {
    gold();
  }
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
  if (x_0 == x_1) {
    vertical_line(x_0, y_0, x_1, y_1, grid);
    return;
  }

  double m = (y_1 - y_0)*1.0/(x_1 - x_0)*1.0;

  if (x_0 > x_1) {
    // Swap points so we only have to consider 4 octants
    bresenham(x_1, y_1, x_0, y_0, grid);
  } else {
    if ((0 <= m) && (m <= 1)) {
      first_octant_bresenham(x_0, y_0, x_1, y_1, grid);
    } else if (1 < m) {
      second_octant_bresenham(x_0, y_0, x_1, y_1, grid);
    } else if (-1 > m) {
      seventh_octant_bresenham(x_0, y_0, x_1, y_1, grid);
    } else if ((0 > m) && (m >= -1)) {
      eighth_octant_bresenham(x_0, y_0, x_1, y_1, grid);
    } else {
      throw "No octant found";
    }
  }
}

void vertical_line(int x_0, int y_0, int x_1, int y_1, Pixel **grid) {
  int y_small = y_0 > y_1 ? y_1 : y_0;
  int y_big = y_0 > y_1 ? y_0 : y_1;
  for (int y = y_small; y <= y_big; y++) {
    fill(x_0, y, grid);
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
  grid[y][x].colored = true;
}
