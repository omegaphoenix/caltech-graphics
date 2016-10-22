#ifndef DRAW_PIXELS_HPP
#define DRAW_PIXELS_HPP

#include <memory> // shared_ptr
#include <vector>

#include "camera.hpp"
#include "light.hpp"
#include "normal.hpp"
#include "three_d_model.hpp"
#include "vertex.hpp"

using namespace std;

struct Material;
struct ThreeDModel;

using CameraPtr = shared_ptr<Camera>;
using LightPtr = shared_ptr<Light>;
using MaterialPtr = shared_ptr<Material>;
using NormalPtr = shared_ptr<Normal>;
using ReflectPtr = shared_ptr<struct Reflectance>;
using ThreeDModelPtr = shared_ptr<ThreeDModel>;
using VertexPtr = shared_ptr<Vertex>;

using LightVecPtr = shared_ptr<vector<LightPtr>>;
using ModelVectorPtr = shared_ptr<vector<ThreeDModelPtr>>;
using VerVectorPtr = shared_ptr<vector<VertexPtr>>;

struct Pixel {
  int red, green, blue;

  Pixel() {
    red = 1;
    green = 1;
    blue = 1;
  }

  Pixel(int r, int g, int b) {
    red = r;
    green = g;
    blue = b;
  }
};

struct ColorVertex {
  VertexPtr ver;
  Pixel col;

  ColorVertex(VertexPtr vertex, Pixel color) {
    ver = vertex;
    col = color;
  }
};

void gouraud(ModelVectorPtr models, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid);
double **new_buffer(int xres, int yres);
void delete_buffer(int xres, int yres, double **buffer);
void gouraud_faces(ThreeDModelPtr model, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);

void raster_tri(ColorVertex NDC_a, ColorVertex NDC_b, ColorVertex NDC_c, int xres, int yres, Pixel **grid, double **buffer);

Eigen::MatrixXd cross_product_vec(Eigen::MatrixXd vec_u, Eigen::MatrixXd vec_v);
VertexPtr create_NDC_point(double alpha, double beta, double gamma, VertexPtr a, VertexPtr b, VertexPtr c);

bool inside_NDC_cube(double alpha, double beta, double gamma, VertexPtr a, VertexPtr b, VertexPtr c);
bool valid_alpha_beta_gamma(double alpha, double beta, double gamma);
bool in_range(double val, double low, double high);

double compute_alpha(VertexPtr a, VertexPtr b, VertexPtr c, int x, int y);
double compute_beta(VertexPtr a, VertexPtr b, VertexPtr c, int x, int y);
double compute_gamma(VertexPtr a, VertexPtr b, VertexPtr c, int x, int y);

int min_x_coord(VertexPtr a, VertexPtr b, VertexPtr c);
int max_x_coord(VertexPtr a, VertexPtr b, VertexPtr c);
int min_y_coord(VertexPtr a, VertexPtr b, VertexPtr c);
int max_y_coord(VertexPtr a, VertexPtr b, VertexPtr c);

double f_ij(VertexPtr i, VertexPtr j, double x, double y);

Pixel lighting(VertexPtr v, NormalPtr n, MaterialPtr material, LightVecPtr lights, CameraPtr cam);
double vec_distance(Eigen::MatrixXd diff_mat);
Eigen::MatrixXd ref_to_mat(ReflectPtr reflect);
Eigen::MatrixXd ver_to_mat(VertexPtr ver);
Eigen::MatrixXd norm_to_mat(NormalPtr norm);
Eigen::MatrixXd normalize_vec(Eigen::MatrixXd mat);
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
 *
 * To generalize Bresenham to the 2nd octant, we switched x and y in the
 * algorithm since each y row has exactly one pixel.
 * To generalize to the 8th octant, we went right and down instead of
 * right and up.  x still increased as normal but when epsilon decreased
 * to less than -dx/2, we decremented y.
 * To generalize to the 7th octant, we switched x and y in the algorithm
 * used for the 8th octant.
 * For the 3rd octant, we reversed the vertices to get the 7th octant.
 * For the 4th octant, we reversed the vertices to get the 8th octant.
 * For the 5th octant, we reversed the vertices to get the 1st octant.
 * For the 6th octant, we reversed the vertices to get the 2nd octant.
 */

// Return true if vertex is within (0, xres) and (0, yres)
bool is_on_screen(VertexPtr v, int xres, int yres);

// Use algorithm from lecture notes without floating point operations
void first_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
// Switch x and y from first_octant_bresenham()
void second_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
// Switch x and y from eighth_octant_bresenham()
void seventh_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);
// Slight modification to first octant algorithm for negative slope
void eighth_octant_bresenham(int x_0, int y_0, int x_1, int y_1, Pixel **grid);

// Indicate that this pixel should be colored
void fill(int x, int y, Pixel **grid, Pixel color);

#endif
