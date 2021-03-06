#ifndef SHADING_HPP
#define SHADING_HPP

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

// Struct which has a vertex and color
struct ColorVertex {
  VertexPtr ver;
  Pixel col;

  ColorVertex(VertexPtr vertex, Pixel color) {
    ver = vertex;
    col = color;
  }
};

// Set 2D pixel array using Phong shading
void phong(ModelVectorPtr models, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid);
// Set 2D pixel array using Gouraud shading
void gouraud(ModelVectorPtr models, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid);

// Use phong shading on single model copy
void phong_faces(ThreeDModelPtr model, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);
// Use phong shading on single triangular face
void phong_shading(ThreeDModelPtr model, FacePtr face, MaterialPtr material, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);
// Use gouraud shading on single model copy
void gouraud_faces(ThreeDModelPtr model, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);
// Use gouraud shading on single triangular face
void gouraud_shading(ThreeDModelPtr model, FacePtr face, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);

// Combine 3 normals by weight
NormalPtr combine_normals(double alpha, double beta, double gamma, NormalPtr n_a, NormalPtr n_b, NormalPtr n_c);
// Combine 3 vertices by weight
VertexPtr combine_vertices(double alpha, double beta, double gamma, VertexPtr v_a, VertexPtr v_b, VertexPtr v_c);

// Rasterize triangle in Pixel grid
void raster_tri(ColorVertex NDC_a, ColorVertex NDC_b, ColorVertex NDC_c, int xres, int yres, Pixel **grid, double **buffer);
// Calculate the lighting of the material at a vertex
Pixel lighting(VertexPtr v, NormalPtr n, MaterialPtr material, LightVecPtr lights, CameraPtr cam);

// Check if face faces away from camera
bool backfacing(VertexPtr NDC_a, VertexPtr NDC_b, VertexPtr NDC_c);
// Check if vertices are inside the NDC cube
bool inside_NDC_cube(VertexPtr NDC);
// Check if alpha, beta, gamma values are within the triangle
bool valid_alpha_beta_gamma(double alpha, double beta, double gamma);
// Check if val between low and high
bool in_range(double val, double low, double high);

// Compute alpha, beta, gamma of coordinates within set of vertices
double compute_alpha(VertexPtr a, VertexPtr b, VertexPtr c, int x, int y);
double compute_beta(VertexPtr a, VertexPtr b, VertexPtr c, int x, int y);
double compute_gamma(VertexPtr a, VertexPtr b, VertexPtr c, int x, int y);
// Helper function for computing alpha, beta, gamma
double f_ij(VertexPtr i, VertexPtr j, double x, double y);

// Calculate the min and max coordinates of the three vertices within the grid
int min_x_coord(VertexPtr a, VertexPtr b, VertexPtr c);
int max_x_coord(VertexPtr a, VertexPtr b, VertexPtr c, int xres);
int min_y_coord(VertexPtr a, VertexPtr b, VertexPtr c);
int max_y_coord(VertexPtr a, VertexPtr b, VertexPtr c, int yres);

// Convert to matrix (3d vector)
Eigen::MatrixXd ref_to_mat(ReflectPtr reflect);
Eigen::MatrixXd ver_to_mat(VertexPtr ver);
Eigen::MatrixXd norm_to_mat(NormalPtr norm);

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
