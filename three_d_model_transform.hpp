#ifndef THREE_D_MODEL_TRANSFORM_HPP
#define THREE_D_MODEL_TRANSFORM_HPP

#include <sstream>
#include <string>
#include <vector>

#include "camera.hpp"
#include "three_d_model.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

struct ThreeDModelTransform {
  ThreeDModel *model;
  int copy_num;
  string name;

  Camera *cam;

  // Perform geometric transforms on vertices
  vector<Vertex *> *transform_model_vertices(Eigen::MatrixXd *trans_mat) {
    vector<Vertex *> *vertices = new vector<Vertex *>();
    // Index 0 is NULL because vertices are 1-indexed
    vertices->push_back(NULL);

    vector<Vertex *>::iterator vertex_it = ++(model->vertices->begin());
    while (vertex_it != model->vertices->end()) {
      vertices->push_back(transform_vertex(trans_mat, *vertex_it));
      ++vertex_it;
    }

    return vertices;
  }

  // Perform geometric and camera perspective transforms on vertices
  vector<Vertex *> *cartesian_NDC(Eigen::MatrixXd *trans_mat) {
    vector<Vertex *> *vertices = new vector<Vertex *>();
    vector<Vertex *> *geo_transformed_vertices = transform_model_vertices(trans_mat);

    // Index 0 is NULL because vertices are 1-indexed
    vertices->push_back(NULL);

    vector<Vertex *>::iterator vertex_it = ++(geo_transformed_vertices->begin());
    while (vertex_it != geo_transformed_vertices->end()) {
      vertices->push_back(cam->cam_transform(*vertex_it));
      ++vertex_it;
    }

    return vertices;
  }

  // Apply all transformations to the vertices to cartesian NDC
  ThreeDModel *apply_trans_mat(Eigen::MatrixXd *trans_mat) {
    ThreeDModel *copy = new ThreeDModel();
    copy->vertices = cartesian_NDC(trans_mat);
    copy->faces = model->faces;

    std::stringstream copy_name;
    copy_name << name << "_copy" << (++copy_num);
    copy->name = copy_name.str();

    return copy;
  }
};

#endif
