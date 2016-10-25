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
using VertexPtr = shared_ptr<Vertex>;
using VerVectorPtr = shared_ptr<vector<VertexPtr>>;

class ModelTransform {
  public:
    ModelPtr model;
    int copy_num;
    string name;

    CameraPtr cam;

    // Perform geometric transforms on vertices
    VerVectorPtr transform_model_vertices(MatrixPtr trans_mat);
    // Perform geometric transforms on normals
    NormVectorPtr transform_model_normals(MatrixPtr trans_mat);

    // Perform geometric and camera perspective transforms on vertices
    VerVectorPtr cartesian_NDC(MatrixPtr trans_mat);

    // Apply all transformations to the vertices to cartesian NDC
    ModelPtr apply_trans_mat(MatrixPtr trans_mat, MatrixPtr norm_trans_mat);
};

#endif
