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

// Return current rotation as GLdouble array
GLdouble *get_current_rotation(Eigen::Quaterniond curr_rotation, Eigen::Quaterniond last_rotation);

// Convert to rotation matrix
Eigen::MatrixXd quaternion_to_rot_mat(int x, int y, int z, int s);
Eigen::Quaterniond compute_rotation_quaternion(int x0, int y0, int x1, int y1, int xres, int yres);

// Get rotation vector and angle from start and end points
Eigen::Vector3d get_unit_rotation_vec(Eigen::Vector3d v0, Eigen::Vector3d v1);
double get_rotation_angle(Eigen::Vector3d v0, Eigen::Vector3d v1);

// Creates NDC coordinate from x and y screen coordinate
Eigen::Vector3d create_NDC(int x, int y, int xres, int yres);
// Helper function for deriving NDC x or y values
double screen_to_NDC(int screen_coord, int coord_res);

#endif
