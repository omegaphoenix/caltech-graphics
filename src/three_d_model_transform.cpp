#include "three_d_model_transform.hpp"

#include <memory> // shared_ptr
#include <sstream>
#include <string>
#include <vector>

#include "camera.hpp"
#include "three_d_model.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

using CameraPtr = shared_ptr<Camera>;
using ThreeDModelPtr = shared_ptr<ThreeDModel>;
using MatrixPtr = shared_ptr<Eigen::MatrixXd>;
using VertexPtr = shared_ptr<Vertex>;

using VerVectorPtr = shared_ptr<vector<VertexPtr>>;

// Perform geometric transforms on vertices
VerVectorPtr ThreeDModelTransform :: transform_model_vertices(MatrixPtr trans_mat) {
  VerVectorPtr vertices = VerVectorPtr(new vector<VertexPtr>());
  // Index 0 is NULL because vertices are 1-indexed
  vertices->push_back(NULL);

  vector<VertexPtr>::iterator vertex_it = ++(model->vertices->begin());
  while (vertex_it != model->vertices->end()) {
    vertices->push_back(transform_vertex(trans_mat, *vertex_it));
    ++vertex_it;
  }

  return vertices;
}

// Perform geometric and camera perspective transforms on vertices
VerVectorPtr ThreeDModelTransform :: cartesian_NDC(MatrixPtr trans_mat) {
  VerVectorPtr vertices = VerVectorPtr(new vector<VertexPtr>());
  VerVectorPtr geo_transform_vertices = transform_model_vertices(trans_mat);

  // Index 0 is NULL because vertices are 1-indexed
  vertices->push_back(NULL);

  vector<VertexPtr>::iterator vertex_it = ++(geo_transform_vertices->begin());
  while (vertex_it != geo_transform_vertices->end()) {
    vertices->push_back(cam->cam_transform(*vertex_it));
    ++vertex_it;
  }

  return vertices;
}

// Apply all transformations to the vertices to cartesian NDC
ThreeDModelPtr ThreeDModelTransform :: apply_trans_mat(MatrixPtr trans_mat) {
  ThreeDModelPtr copy = ThreeDModelPtr(new ThreeDModel());
  copy->vertices = transform_model_vertices(trans_mat);
  // copy->vertices = cartesian_NDC(trans_mat);
  copy->faces = model->faces;

  std::stringstream copy_name;
  copy_name << name << "_copy" << (++copy_num);
  copy->name = copy_name.str();

  return copy;
}
