#ifndef SHADING_HPP
#define SHADING_HPP

#include <memory> // shared_ptr
#include <vector>

#include "camera.hpp"
#include "light.hpp"
#include "normal.hpp"
#include "model.hpp"
#include "vertex.hpp"

using namespace std;

struct Material;
struct Model;

using CameraPtr = shared_ptr<Camera>;
using LightPtr = shared_ptr<Light>;
using MaterialPtr = shared_ptr<Material>;
using NormalPtr = shared_ptr<Normal>;
using ReflectPtr = shared_ptr<struct Reflectance>;

using LightVecPtr = shared_ptr<vector<Light>>;

// Struct which has a vertex and color
struct ColorVertex {
  Vertex ver;
  Pixel col;

  ColorVertex(Vertex vertex, Pixel color) {
    ver = vertex;
    col = color;
  }
};

// Set 2D pixel array using Phong shading
void phong(vector<Model> models, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid);
// Set 2D pixel array using Gouraud shading
void gouraud(vector<Model> models, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid);

// Use phong shading on single model copy
void phong_faces(Model model, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);
// Use phong shading on single triangular face
void phong_shading(Model model, FacePtr face, MaterialPtr material, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);
// Use gouraud shading on single model copy
void gouraud_faces(Model model, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);
// Use gouraud shading on single triangular face
void gouraud_shading(Model model, FacePtr face, vector<Light> lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);

// Combine 3 normals by weight
Normal combine_normals(double alpha, double beta, double gamma, Normal n_a, Normal n_b, Normal n_c);
// Combine 3 vertices by weight
Vertex combine_vertices(double alpha, double beta, double gamma, Vertex v_a, Vertex v_b, Vertex v_c);

// Rasterize triangle in Pixel grid
void raster_tri(ColorVertex NDC_a, ColorVertex NDC_b, ColorVertex NDC_c, int xres, int yres, Pixel **grid, double **buffer);
// Calculate the lighting of the material at a vertex
Pixel lighting(Vertex v, Normal n, MaterialPtr material, vector<Light> lights, CameraPtr cam);

// Check if face faces away from camera
bool backfacing(Vertex NDC_a, Vertex NDC_b, Vertex NDC_c);
// Check if vertices are inside the NDC cube
bool inside_NDC_cube(Vertex NDC);
// Check if alpha, beta, gamma values are within the triangle
bool valid_alpha_beta_gamma(double alpha, double beta, double gamma);
// Check if val between low and high
bool in_range(double val, double low, double high);

// Compute alpha, beta, gamma of coordinates within set of vertices
double compute_alpha(Vertex a, Vertex b, Vertex c, int x, int y);
double compute_beta(Vertex a, Vertex b, Vertex c, int x, int y);
double compute_gamma(Vertex a, Vertex b, Vertex c, int x, int y);
// Helper function for computing alpha, beta, gamma
double f_ij(Vertex i, Vertex j, double x, double y);

// Calculate the min and max coordinates of the three vertices within the grid
int min_x_coord(Vertex a, Vertex b, Vertex c);
int max_x_coord(Vertex a, Vertex b, Vertex c, int xres);
int min_y_coord(Vertex a, Vertex b, Vertex c);
int max_y_coord(Vertex a, Vertex b, Vertex c, int yres);

// Convert to matrix (3d vector)
Eigen::MatrixXd ref_to_mat(ReflectPtr reflect);
Eigen::MatrixXd ver_to_mat(Vertex ver);
Eigen::MatrixXd norm_to_mat(Normal norm);

// Normalize 3D vector
Eigen::MatrixXd normalize_vec(Eigen::MatrixXd mat);
// Take the cross product of 3D vectors
Eigen::MatrixXd cross_product_vec(Eigen::MatrixXd vec_u, Eigen::MatrixXd vec_v);
// Calculates the magnitude of a 3d vector
double magnitude(Eigen::MatrixXd diff_mat);

// Create 2D array for keeping track of z-coordinate of pixels
double **new_buffer(int xres, int yres);
// Delete 2D array for keeping track of z-coordinate of pixels
void delete_buffer(int xres, int yres, double **buffer);
#endif
