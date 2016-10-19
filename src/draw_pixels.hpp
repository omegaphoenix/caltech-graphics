#ifndef DRAW_PIXELS_HPP
#define DRAW_PIXELS_HPP

#include <memory> // shared_ptr
#include <vector>

#include "vertex.hpp"

using namespace std;

using VertexPtr = shared_ptr<Vertex>;
using VerVectorPtr = shared_ptr<vector<VertexPtr>>;

struct Pixel {
  bool colored;
};

// Print .ppm file
void output_ppm(int xres, int yres, Pixel **grid);
// Helper method for start of .ppm output
void start_ppm_output(int xres, int yres);
// Helper method for outputing a line for a single pixel
void output_pixel(int row, int col, Pixel **grid);

// Output line representing pixel of the specified color
void purple();
void gold();

// Converts vertices from cartesian NDC to screen location
VerVectorPtr NDCs_to_pixels(int xres, int yres, VerVectorPtr ndc_vertices);
// Converts one vertex from cartesian NDC to screen location
VertexPtr NDC_to_pixel(int xres, int yres, VertexPtr ndc_vertex);

// Draw line between vertices on Pixel array
void rasterize(VertexPtr v1, VertexPtr v2, Pixel **grid, int xres, int yres);
// Swap points if vector pointing left (octants 3-6)
void bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
/*
 *  Octants:
 *    \3|2/
 *    4\|/1
 *   ---+---
 *    5/|\8
 *    /6|7\
 */

// Return true if vertex is within (0, xres) and (0, yres)
bool is_on_screen(VertexPtr v, int xres, int yres);

void vertical_line(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
// Use algorithm from lecture notes without floating point operations
void first_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
// Switch x and y from first_octant_bresenham()
void second_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
// Switch x and y from eighth_octant_bresenham()
void seventh_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
// Slight modification to first octant algorithm for negative slope
void eighth_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);

// Indicate that this pixel should be colored
void fill(int x, int y, Pixel **grid);

#endif
