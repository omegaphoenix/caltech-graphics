#include "arcball.hpp"

#include <GL/glew.h>
#include <iostream>
#include <math.h>
#define _USE_MATH_DEFINES


#include "Eigen/Dense"

using namespace std;

GLdouble *get_current_rotation(Eigen::Quaterniond curr_rotation, Eigen::Quaterniond last_rotation) {
  Eigen::Quaterniond res = curr_rotation * last_rotation;
  double x = res.x();
  double y = res.y();
  double z = res.z();
  double s = res.w();

  Eigen::MatrixXd rot_mat = quaternion_to_rot_mat(x, y, z, s);

  // Return rotation matrix as opengl matrix
  GLdouble *out = new GLdouble[16];
  int k = 0;
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      // opengl matrix is column-major
      out[k] = rot_mat(col, row);
      k++;
    }
  }
  return out;
}

Eigen::MatrixXd quaternion_to_rot_mat(double x, double y, double z, double s) {
  Eigen::MatrixXd rot_mat(4, 4);
  rot_mat << 1 - 2*y*y - 2*z*z, 2*(x*y - z*s), 2*(x*z + y*s), 0,
             2*(x*y + z*s), 1 - 2*x*x - 2*z*z, 2*(y*z - x*s), 0,
             2*(x*z - y*s), 2*(y*z + x*s), 1 - 2*x*x - 2*y*y, 0,
             0, 0, 0, 1;
  return rot_mat;
}

Eigen::Quaterniond compute_rotation_quaternion(int x0, int y0, int x1, int y1, int xres, int yres) {
  // Get NDC coordinates from screen coordinates
  Eigen::Vector3d v0 = create_NDC(x0, y0, xres, yres);
  Eigen::Vector3d v1 = create_NDC(x1, y1, xres, yres);

  // Get rotation angle and axis
  double theta = get_rotation_angle(v0, v1);
  Eigen::Vector3d rotation_vec = get_unit_rotation_vec(v0, v1);

  // Compute values for unit quaternion
  double quaternion_real = cos(theta/2.0);
  double imag_scalar = sin(theta/2.0);
  Eigen::Vector3d quaternion_imag = rotation_vec * imag_scalar;

  // Create rotation quaternion
  Eigen::Quaterniond q(quaternion_real, quaternion_imag(0, 0),
      quaternion_imag(1, 0), quaternion_imag(2, 0));
  return q;
}

Eigen::Vector3d get_unit_rotation_vec(Eigen::Vector3d v0, Eigen::Vector3d v1) {
  Eigen::Vector3d unit_rotation_vec = v0.cross(v1);
  if (unit_rotation_vec.norm() != 0) {
    unit_rotation_vec.normalize();
  }
  return unit_rotation_vec;
}

double get_rotation_angle(Eigen::Vector3d v0, Eigen::Vector3d v1) {
  double denom = v0.norm() * v1.norm();
  double rot_ang_help = (denom > 0) ? v0.dot(v1) / denom : 0;
  double theta = acos(min(1.0, rot_ang_help));
  return theta;
}

Eigen::Vector3d create_NDC(int x, int y, int xres, int yres) {
  double NDC_x = screen_to_NDC(x, xres);
  double NDC_y = -screen_to_NDC(y, yres);

  // Compute z coordinate from x and y
  double NDC_z_squared = 1 - NDC_x*NDC_x - NDC_y*NDC_y;
  double NDC_z = (NDC_z_squared > 0) ? sqrt(NDC_z_squared) : 0;

  Eigen::Vector3d ver(NDC_x, NDC_y, NDC_z);
  return ver;
}

double screen_to_NDC(int screen_coord, int screen_res) {
  double NDC = (2.0 * screen_coord / screen_res) - 1.0;
  return NDC;
}

