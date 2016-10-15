#ifndef DRAW_PIXELS_HPP
#define DRAW_PIXELS_HPP

#include <memory>
#include <vector>

#include "vertex.hpp"

using namespace std;

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
shared_ptr<vector<shared_ptr<Vertex>>> NDCs_to_pixels(int xres, int yres, shared_ptr<vector<shared_ptr<Vertex>>> ndc_vertices);
// Converts one vertex from cartesian NDC to screen location
shared_ptr<Vertex> NDC_to_pixel(int xres, int yres, shared_ptr<Vertex> ndc_vertex);

// Draw line between vertices on Pixel array
void rasterize(shared_ptr<Vertex> v1, shared_ptr<Vertex> v2, Pixel **grid, int xres, int yres);
void bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);

// Return true if vertex is within (0, xres) and (0, yres)
bool is_on_screen(shared_ptr<Vertex> v, int xres, int yres);

void vertical_line(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
// Use algorithm from lecture notes
void first_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
// Switch x and y from first_octant_bresenham()
void second_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
// Switch x and y from eighth_octant_bresenham()
void seventh_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
// Slight modification to first octant algorithm for negative slope
void eighth_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);

// These call the *_octant_bresenham function of the opposite octant by swapping the two vertices
void third_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
void fourth_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
void fifth_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
void sixth_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);

// Indicate that this pixel should be colored
void fill(int x, int y, Pixel **grid);

#endif
