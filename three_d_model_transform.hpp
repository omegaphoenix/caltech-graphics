#ifndef THREE_D_MODEL_TRANSFORM_HPP
#define THREE_D_MODEL_TRANSFORM_HPP

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "camera.hpp"
#include "three_d_model.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

struct ThreeDModelTransform {
  shared_ptr<ThreeDModel> model;
  int copy_num;
  string name;

  shared_ptr<Camera> cam;

  // Perform geometric transforms on vertices
  vector<shared_ptr<Vertex> > *transform_model_vertices(shared_ptr<Eigen::MatrixXd> trans_mat) {
    vector<shared_ptr<Vertex> > *vertices = new vector<shared_ptr<Vertex> >();
    // Index 0 is NULL because vertices are 1-indexed
    vertices->push_back(NULL);

    vector<shared_ptr<Vertex> >::iterator vertex_it = ++(model->vertices->begin());
    while (vertex_it != model->vertices->end()) {
      vertices->push_back(transform_vertex(trans_mat, *vertex_it));
      ++vertex_it;
    }

    return vertices;
  }

  // Perform geometric and camera perspective transforms on vertices
  vector<shared_ptr<Vertex> > *cartesian_NDC(shared_ptr<Eigen::MatrixXd> trans_mat) {
    vector<shared_ptr<Vertex> > *vertices = new vector<shared_ptr<Vertex> >();
    vector<shared_ptr<Vertex> > *geo_transformed_vertices = transform_model_vertices(trans_mat);

    // Index 0 is NULL because vertices are 1-indexed
    vertices->push_back(NULL);

    vector<shared_ptr<Vertex> >::iterator vertex_it = ++(geo_transformed_vertices->begin());
    while (vertex_it != geo_transformed_vertices->end()) {
      vertices->push_back(cam->cam_transform(*vertex_it));
      ++vertex_it;
    }

    geo_transformed_vertices->clear();
    delete geo_transformed_vertices;

    return vertices;
  }

  // Apply all transformations to the vertices to cartesian NDC
  shared_ptr<ThreeDModel> apply_trans_mat(shared_ptr<Eigen::MatrixXd> trans_mat) {
    shared_ptr<ThreeDModel> copy = shared_ptr<ThreeDModel>(new ThreeDModel());
    copy->vertices = cartesian_NDC(trans_mat);
    copy->faces = model->faces;

    std::stringstream copy_name;
    copy_name << name << "_copy" << (++copy_num);
    copy->name = copy_name.str();

    return copy;
  }
};

#endif
