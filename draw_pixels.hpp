#ifndef DRAW_PIXELS_HPP
#define DRAW_PIXELS_HPP

#include <vector>

#include "vertex.hpp"

using namespace std;

struct Pixel {
  int red;
  int blue;
  int yellow;
};

vector<Vertex *> *NDCs_to_pixels(int xres, int yres, vector<Vertex *> *ndc_vertices);
Vertex *NDC_to_pixel(int xres, int yres, Vertex *ndc_vertex);
void bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
void first_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
void second_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
void third_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
void fourth_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
void fifth_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
void sixth_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
void seventh_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
void eighth_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);

void fill(int x, int y, Pixel **grid);

#endif
