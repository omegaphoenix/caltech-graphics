#ifndef THREE_D_MODEL_TRANSFORM_HPP
#define THREE_D_MODEL_TRANSFORM_HPP

#include <sstream>
#include <string>
#include <vector>

#include "three_d_model.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

struct ThreeDModelTransform {
  ThreeDModel *model;
  int copy_num;
  string name;

  Vertex *transform_vertex(Eigen::MatrixXd *trans_mat, Vertex *vertex) {
    Eigen::MatrixXd *vertex_mat = new Eigen::MatrixXd(4, 1);
    *vertex_mat << vertex->x, // row1
                   vertex->y, // row2
                   vertex->z, // row3
                   1;         // row4

    Eigen::MatrixXd transformed = *trans_mat * *vertex_mat;
    double new_x = transformed(0) / transformed(3);
    double new_y = transformed(1) / transformed(3);
    double new_z = transformed(2) / transformed(3);
    return new Vertex(new_x, new_y, new_z);
  }

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

  ThreeDModel *apply_trans_mat(Eigen::MatrixXd *trans_mat) {
    ThreeDModel *copy = new ThreeDModel();
    copy->vertices = transform_model_vertices(trans_mat);
    copy->faces = model->faces;

    std::stringstream copy_name;
    copy_name << name << "_copy" << (++copy_num);
    copy->name = copy_name.str();

    return copy;
  }
};

#endif
