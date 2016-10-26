#ifndef MODEL_TRANSFORM_HPP
#define MODEL_TRANSFORM_HPP

#include <memory> // shared_ptr
#include <sstream>
#include <string>
#include <vector>

#include "camera.hpp"
#include "model.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

using CameraPtr = shared_ptr<Camera>;
using ModelPtr = shared_ptr<Model>;
using MatrixPtr = shared_ptr<Eigen::MatrixXd>;

class ModelTransform {
  public:
    Model model;
    int copy_num;
    string name;

    CameraPtr cam;

    // Perform geometric transforms on vertices
    vector<Vertex> transform_model_vertices(MatrixPtr trans_mat);
    // Perform geometric transforms on normals
    NormVectorPtr transform_model_normals(MatrixPtr trans_mat);

    // Perform geometric and camera perspective transforms on vertices
    vector<Vertex> cartesian_NDC(MatrixPtr trans_mat);

    // Apply all transformations to the vertices to cartesian NDC
    Model apply_trans_mat(MatrixPtr trans_mat, MatrixPtr norm_trans_mat);
};

#endif
