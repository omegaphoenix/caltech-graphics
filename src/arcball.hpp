#ifndef ARCBALL_HPP
#define ARCBALL_HPP

#include <GL/glew.h>
#include <math.h>
#define _USE_MATH_DEFINES

// iostream and vector are standard libraries that are just generally useful.
#include <iostream>
#include <vector>

#include "Eigen/Dense"

using namespace std;

Eigen::Vector3d create_NDC(int x, int y);
Eigen::Quaterniond compute_rotation_quaternion(int x0, int y0, int x1, int y1, int xres, int yres);
GLdouble *get_current_rotation(Eigen::Quaterniond curr_rotation, Eigen::Quaterniond last_rotation);
Eigen::Vector3d get_unit_rotation_axis(Eigen::Vector3d v0, Eigen::Vector3d v1);
double get_rotation_angle(int x0, int y0, int x1, int y1);
double screen_to_NDC(int screen_coord, int coord_res);

#endif
