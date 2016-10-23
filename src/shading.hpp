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

struct ColorVertex {
  VertexPtr ver;
  Pixel col;

  ColorVertex(VertexPtr vertex, Pixel color) {
    ver = vertex;
    col = color;
  }
};

void phong(ModelVectorPtr models, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid);
void phong_faces(ThreeDModelPtr model, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);
void phong_shading(VertexPtr v_a, VertexPtr v_b, VertexPtr v_c, NormalPtr n_a, NormalPtr n_b, NormalPtr n_c, MaterialPtr material, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);
NormalPtr combine_normals(double alpha, double beta, double gamma, NormalPtr n_a, NormalPtr n_b, NormalPtr n_c);
VertexPtr combine_vertices(double alpha, double beta, double gamma, VertexPtr v_a, VertexPtr v_b, VertexPtr v_c);
void gouraud(ModelVectorPtr models, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid);
double **new_buffer(int xres, int yres);
void delete_buffer(int xres, int yres, double **buffer);
void gouraud_faces(ThreeDModelPtr model, LightVecPtr lights, CameraPtr cam, int xres, int yres, Pixel **grid, double **buffer);

void raster_tri(ColorVertex NDC_a, ColorVertex NDC_b, ColorVertex NDC_c, int xres, int yres, Pixel **grid, double **buffer);

Eigen::MatrixXd cross_product_vec(Eigen::MatrixXd vec_u, Eigen::MatrixXd vec_v);
VertexPtr create_NDC_point(double alpha, double beta, double gamma, VertexPtr a, VertexPtr b, VertexPtr c);

bool inside_NDC_cube(VertexPtr NDC);
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
#endif
