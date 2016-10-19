#include "camera.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "geometric_transform.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

using VertexPtr = shared_ptr<Vertex>;
using MatrixPtr = shared_ptr<Eigen::MatrixXd>;

// Parse lines for setting up camera transforms
Camera :: Camera(vector<string> lines) {
  set_position(lines[1]);
  set_orient(lines[2]);
  set_perspective(lines);

  calc_position_mat();
  calc_perspect_mat();
}

// Returns Cartesian normalized device coordinates (NDC)
VertexPtr Camera :: cam_transform(VertexPtr v) {
  VertexPtr transformed = transform_vertex(cam_transform_mat, v);

  transformed = transform_vertex(perspective_proj_mat, transformed);
  return transformed;
}

// Parse position translation line
void Camera :: set_position(string line) {
  istringstream line_stream(line);

  string _;
  double x, y, z;
  if (!(line_stream >> _ >> x >> y >> z)) {
    throw "Wrong number of arguments to camera position line";
  }

  pos = VertexPtr(new Vertex(x, y, z));
}

// Parse position orientation rotation line
void Camera :: set_orient(string line) {
  istringstream line_stream(line);

  string orientation;
  double x, y, z;
  if (!(line_stream >> orientation >> x >> y >> z >> orient_angle)) {
    throw "Wrong number of arguments to camera orientation line";
  }

  orient = VertexPtr(new Vertex(x, y, z));
}

// Get all arguments for perspective projection transform
void Camera :: set_perspective(vector<string> lines) {
  istringstream near_line_stream(lines[3]);
  string _;
  if (!(near_line_stream >> _ >> near)) {
    throw "Wrong number of arguments to camera perspective near line";
  }

  istringstream far_line_stream(lines[4]);
  if (!(far_line_stream >> _ >> far)) {
    throw "Wrong number of arguments to camera perspective far line";
  }

  istringstream left_line_stream(lines[5]);
  if (!(left_line_stream >> _ >> left)) {
    throw "Wrong number of arguments to camera perspective left line";
  }

  istringstream right_line_stream(lines[6]);
  if (!(right_line_stream >> _ >> right)) {
    throw "Wrong number of arguments to camera perspective right line";
  }

  istringstream top_line_stream(lines[7]);
  if (!(top_line_stream >> _ >> top)) {
    throw "Wrong number of arguments to camera perspective top line";
  }

  istringstream bottom_line_stream(lines[8]);
  if (!(bottom_line_stream >> _ >> bottom)) {
    throw "Wrong number of arguments to camera perspective bottom line";
  }
}

// Calculate position transform matrix from translation and rotation.
void Camera :: calc_position_mat() {
  MatrixPtr cam_translate_mat =
    create_translation_mat(pos->x, pos->y, pos->z);
  MatrixPtr cam_rotate_mat =
    create_rotation_mat(orient->x, orient->y, orient->z, orient_angle);

  cam_transform_mat = MatrixPtr(new Eigen::MatrixXd(4, 4));
  *cam_transform_mat = (*cam_translate_mat * *cam_rotate_mat).inverse();
}

// Calculate perspective transform matrix
void Camera :: calc_perspect_mat() {
  double p_11 = 2*near/(right - left);
  double p_13 = (right + left)/(right - left);
  double p_22 = 2*near/(top - bottom);
  double p_23 = (top + bottom)/(top - bottom);
  double p_33 = -(far + near)/(far - near);
  double p_34 = -2*far*near/(far - near);

  perspective_proj_mat = MatrixPtr(new Eigen::MatrixXd(4, 4));
  *perspective_proj_mat <<
      p_11, 0, p_13, 0, // row1
      0, p_22, p_23, 0, // row2
      0, 0, p_33, p_34, // row2
      0, 0, -1, 0; // row2
}
