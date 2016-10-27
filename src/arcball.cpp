#include "arcball.hpp"

#include <GL/glew.h>
#include <math.h>
#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>

#include "Eigen/Dense"

using namespace std;

GLdouble *get_current_rotation(Eigen::Quaterniond curr_rotation, Eigen::Quaterniond last_rotation) {
  Eigen::Quaterniond res = curr_rotation * last_rotation;
  double x = res.x();
  double y = res.y();
  double z = res.z();
  double s = res.w();

  Eigen::MatrixXd m(4, 4);
  m << 1 - 2*y*y - 2*z*z, 2*(x*y - z*s), 2*(x*z + y*s), 0,
       2*(x*y + z*s), 1 - 2*x*x - 2*z*z, 2*(y*z - x*s), 0,
       2*(x*z - y*s), 2*(y*z + x*s), 1 - 2*x*x - 2*y*y, 0,
			 0, 0, 0, 1;

  // opengl's matrix is column-major
  GLdouble *out = new GLdouble[16];
  int k = 0;
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      out[k] = m(col, row);
      k++;
    }
  }
  return out;
}

Eigen::Vector3d create_NDC(int x, int y, int xres, int yres) {
  double NDC_x = screen_to_NDC(x, xres);
  double NDC_y = -screen_to_NDC(y, yres);
  double NDC_z_squared = 1 - NDC_x*NDC_x - NDC_y*NDC_y;
  double NDC_z = (NDC_z > 0) ? sqrt(NDC_z_squared) : 0;

  Eigen::Vector3d ver(NDC_x, NDC_y, NDC_z);
  return ver;
}

Eigen::Vector3d get_unit_rotation_axis(Eigen::Vector3d v0, Eigen::Vector3d v1) {
  Eigen::Vector3d unit_rotation_axis = v0.cross(v1);
  if (unit_rotation_axis.norm() != 0)
    unit_rotation_axis.normalize();
  return unit_rotation_axis;
}

double get_rotation_angle(Eigen::Vector3d v0, Eigen::Vector3d v1) {
  double rot_ang_help = v0.dot(v1) / v0.norm() * v1.norm();
  double theta = acos(min(1.0, rot_ang_help));
  return theta;
}

double screen_to_NDC(int screen_coord, int screen_res) {
  double NDC = (2.0 * screen_coord / screen_res) - 1.0;
  return NDC;
}

Eigen::Quaterniond compute_rotation_quaternion(int x0, int y0, int x1, int y1, int xres, int yres) {
  Eigen::Vector3d v0 = create_NDC(x0, y0, xres, yres);
  Eigen::Vector3d v1 = create_NDC(x1, y1, xres, yres);

  double theta = get_rotation_angle(v0, v1);
  Eigen::Vector3d rotation_axis = get_unit_rotation_axis(v0, v1);

  double quaternion_real = cos(theta/2.0);
  double imaginary_scalar = sin(theta/2.0);

  Eigen::Vector3d quaternion_imaginary = rotation_axis * imaginary_scalar;
  Eigen::Quaterniond q(quaternion_real, quaternion_imaginary(0, 0),
      quaternion_imaginary(1, 0), quaternion_imaginary(2, 0));
  return q;
}

