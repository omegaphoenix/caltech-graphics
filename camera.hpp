#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <sstream>
#include <string>
#include <vector>

#include "geometric_transform.hpp"
#include "three_d_model_transform.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

// Represents the camera position, orientation, and perspective
struct Camera {
  Vertex *pos;

  Vertex *orient;
  double orient_angle;

  double near, far, left, right, top, bottom;

  Eigen::MatrixXd *cam_transform_mat, *perspective_proj_mat;

  Camera(vector<string> *lines) {
    set_position((*lines)[1]);
    set_orient((*lines)[2]);
    set_perspective(lines);

    calc_position_mat();
    calc_perspect_mat();
  }

  /* Returns Cartesian normalized device coordinates (NDC) */
  Vertex *cam_transform(Vertex *v) {
    double homogeneous_component = v->z;
    // Vertex *transformed = transform_vertex(cam_transform_mat->inverse(), v);
  }

  void set_position(string line) {
    istringstream line_stream(line);

    string _;
    double x, y, z;
    if (!(line_stream >> _ >> x >> y >> z)) {
      throw "Wrong number of arguments to camera position line";
    }

    pos = new Vertex(x, y, z);
  }

  void set_orient(string line) {
    istringstream line_stream(line);

    string orientation;
    double x, y, z;
    if (!(line_stream >> orientation >> x >> y >> z >> orient_angle)) {
      throw "Wrong number of arguments to camera orientation line";
    }

    orient = new Vertex(x, y, z);
  }

  /* Get all arguments for perspective projection transform */
  void set_perspective(vector<string> *lines) {
    istringstream near_line_stream((*lines)[3]);
    string _;
    if (!(near_line_stream >> _ >> near)) {
      throw "Wrong number of arguments to camera perspective near line";
    }

    istringstream far_line_stream((*lines)[4]);
    if (!(far_line_stream >> _ >> far)) {
      throw "Wrong number of arguments to camera perspective far line";
    }

    istringstream left_line_stream((*lines)[5]);
    if (!(left_line_stream >> _ >> left)) {
      throw "Wrong number of arguments to camera perspective left line";
    }

    istringstream right_line_stream((*lines)[6]);
    if (!(right_line_stream >> _ >> right)) {
      throw "Wrong number of arguments to camera perspective right line";
    }

    istringstream top_line_stream((*lines)[7]);
    if (!(top_line_stream >> _ >> top)) {
      throw "Wrong number of arguments to camera perspective top line";
    }

    istringstream bottom_line_stream((*lines)[8]);
    if (!(bottom_line_stream >> _ >> bottom)) {
      throw "Wrong number of arguments to camera perspective bottom line";
    }
  }

  void calc_position_mat() {
    Eigen::MatrixXd *cam_translate_mat = create_translation_mat(pos->x, pos->y, pos->z);
    Eigen::MatrixXd *cam_rotate_mat = create_rotation_mat(orient->x, orient->y, orient->z, orient_angle);

    cam_transform_mat = new Eigen::MatrixXd(4, 4);
    *cam_transform_mat = *cam_translate_mat * *cam_rotate_mat;
  }

  void calc_perspect_mat() {
    double p_11 = 2*near/(right - left);
    double p_13 = (right + left)/(right - left);
    double p_22 = 2*near/(top - bottom);
    double p_23 = (top + bottom)/(top - bottom);
    double p_33 = -(far + near)/(far - near);
    double p_34 = -2*far*near/(far - near);

    *perspective_proj_mat <<
        p_11, 0, p_13, 0, // row1
        0, p_22, p_23, 0, // row2
        0, 0, p_33, p_34, // row2
        0, 0, -1, 0; // row2
  }

};

#endif
