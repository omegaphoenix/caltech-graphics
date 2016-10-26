#include "model_transform.hpp"

#include <memory> // shared_ptr
#include <sstream>
#include <string>
#include <vector>

#include "camera.hpp"
#include "normal.hpp"
#include "model.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

using CameraPtr = shared_ptr<Camera>;
using ModelPtr = shared_ptr<Model>;
using MatrixPtr = shared_ptr<Eigen::MatrixXd>;

// Perform geometric transforms on vertices
vector<Vertex> ModelTransform :: transform_model_vertices(MatrixPtr trans_mat) {
  vector<Vertex> vertices = vector<Vertex>();
  // Index 0 is filler because vertices are 1-indexed
  vertices.push_back(Vertex());

  vector<Vertex>::iterator vertex_it = ++(model.vertex_buffer.begin());
  while (vertex_it != model.vertex_buffer.end()) {
    vertices.push_back(transform_vertex(trans_mat, *vertex_it));
    ++vertex_it;
  }

  return vertices;
}

// Perform geometric transforms on normals
vector<Normal> ModelTransform :: transform_model_normals(MatrixPtr trans_mat) {
  vector<Normal> normals = vector<Normal>();
  // Index 0 is NULL because normals are 1-indexed
  normals.push_back(Normal());

  vector<Normal>::iterator normal_it = ++(model.normals.begin());
  while (normal_it != model.normals.end()) {
    normals.push_back(transform_normal(trans_mat, *normal_it));
    ++normal_it;
  }

  return normals;
}

// Perform geometric and camera perspective transforms on vertices
vector<Vertex> ModelTransform :: cartesian_NDC(MatrixPtr trans_mat) {
  vector<Vertex> vertices = vector<Vertex>();
  vector<Vertex> geo_transform_vertices = transform_model_vertices(trans_mat);

  // Index 0 is filler because vertices are 1-indexed
  vertices.push_back(Vertex());

  vector<Vertex>::iterator vertex_it = ++(geo_transform_vertices.begin());
  while (vertex_it != geo_transform_vertices.end()) {
    vertices.push_back(cam->cam_transform(*vertex_it));
    ++vertex_it;
  }

  return vertices;
}

// Apply all transformations to the vertices to cartesian NDC
Model ModelTransform :: apply_trans_mat(MatrixPtr trans_mat, MatrixPtr norm_trans_mat) {
  Model copy = Model();
  copy.vertex_buffer = transform_model_vertices(trans_mat);
  copy.normals = transform_model_normals(norm_trans_mat);
  // copy->vertices = cartesian_NDC(trans_mat);
  copy.faces = model.faces;

  std::stringstream copy_name;
  copy_name << name << "_copy" << (++copy_num);
  copy.name = copy_name.str();

  return copy;
}
