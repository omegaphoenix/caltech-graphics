#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "geometric_transform.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

using MatrixPtr = shared_ptr<Eigen::MatrixXd>;

// Represents the camera position, orientation, and perspective
struct Camera {
  Vertex pos, orient;
  float orient_angle;

  double near, far, left, right, top, bottom;

  MatrixPtr cam_transform_mat, perspective_proj_mat;

  // Parse lines for setting up camera transforms
  Camera(vector<string> lines);

  // Returns Cartesian normalized device coordinates (NDC)
  Vertex cam_transform(Vertex v);

  // Parse position translation line
  void set_position(string line);

  // Parse position orientation rotation line
  void set_orient(string line);

  // Get all arguments for perspective projection transform
  void set_perspective(vector<string> lines);

  // Calculate position transform matrix from translation and rotation.
  void calc_position_mat();

  // Calculate perspective transform matrix
  void calc_perspect_mat();
};

#endif
