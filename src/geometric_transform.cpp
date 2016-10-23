#include "geometric_transform.hpp"

#include <fstream> // basic file operations
#include <iostream>
#include <math.h>
#include <memory> // shared_ptr
#include <string>
#include <vector>

#include "normal.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

using NormalPtr = shared_ptr<Normal>;
using MatrixPtr = shared_ptr<Eigen::MatrixXd>;
using VertexPtr = shared_ptr<Vertex>;

MatrixPtr inverse_transform(vector<string> lines) {
  MatrixPtr prod = multiply_matrices(lines);
  MatrixPtr res = MatrixPtr(new Eigen::MatrixXd(4, 4));
  *res =  prod->inverse();
}

vector<string> parse_file_to_line_vector(char *file_name) {
  ifstream obj_file(file_name);
  vector<string> lines;

  string line;
  while (getline(obj_file, line)) {
    lines.push_back(line);
  }

  return lines;
}

MatrixPtr multiply_matrices(vector<string> lines) {
  MatrixPtr prod = MatrixPtr(new Eigen::MatrixXd(4, 4));
  prod->setIdentity(4, 4);

  for (vector<string>::iterator line_it = lines.begin(); line_it != lines.end(); ++line_it) {
    multiply_into_matrix(*line_it, prod);
  }

  return prod;
}

MatrixPtr create_norm_trans_mat(vector<string> lines) {
  MatrixPtr prod = MatrixPtr(new Eigen::MatrixXd(4, 4));
  prod->setIdentity(4, 4);

  for (vector<string>::iterator line_it = lines.begin(); line_it != lines.end(); ++line_it) {
    string line = *line_it;
    if (!is_translation_line(line)) {
      multiply_into_matrix(*line_it, prod);
    }
  }

  MatrixPtr res = MatrixPtr(new Eigen::MatrixXd(4, 4));
  *res = prod->inverse().transpose();

  return prod;
}

void multiply_into_matrix(string line, MatrixPtr mat) {
  MatrixPtr new_mat = parse_transform_line(line);
  *mat = *new_mat * *mat;
}

MatrixPtr parse_transform_line(string line) {
  if (is_translation_line(line)) {
    return parse_translation_line(line);

  } else if (is_rotation_line(line)) {
    return parse_rotation_line(line);

  } else if (is_scaling_line(line)) {
    return parse_scaling_line(line);
  }
}

bool is_translation_line(string line) {
  return line.at(0) == 't';
}

bool is_rotation_line(string line) {
  return line.at(0) == 'r';
}

bool is_scaling_line(string line) {
  return line.at(0) == 's';
}

MatrixPtr parse_scaling_line(string line) {
  istringstream line_stream(line);
  char s;
  double s_x, s_y, s_z;
  if (!(line_stream >> s >> s_x >> s_y >> s_z)) {
    throw "Wrong number of arguments to scaling line";
  }

  return create_scaling_mat(s_x, s_y, s_z);
}

MatrixPtr parse_translation_line(string line) {
  istringstream line_stream(line);
  char t;
  double t_x, t_y, t_z;
  if (!(line_stream >> t >> t_x >> t_y >> t_z)) {
    throw "Wrong number of arguments to translation line";
  }

  return create_translation_mat(t_x, t_y, t_z);
}

MatrixPtr parse_rotation_line(string line) {
  istringstream line_stream(line);
  char r;
  double r_x, r_y, r_z, angle_in_rad;
  if (!(line_stream >> r >> r_x >> r_y >> r_z >> angle_in_rad)) {
    throw "Wrong number of arguments to translation line";
  }

  return create_rotation_mat(r_x, r_y, r_z, angle_in_rad);
}

MatrixPtr create_translation_mat(double t_x, double t_y, double t_z) {
  MatrixPtr mat = MatrixPtr(new Eigen::MatrixXd(4, 4));

  *mat << 1, 0, 0, t_x, // row1
          0, 1, 0, t_y, // row2
          0, 0, 1, t_z, // row3
          0, 0, 0, 1;   // row4

  return mat;
}

MatrixPtr create_scaling_mat(double s_x, double s_y, double s_z) {
  MatrixPtr mat = MatrixPtr(new Eigen::MatrixXd(4, 4));

  *mat << s_x, 0, 0, 0, // row1
          0, s_y, 0, 0, // row2
          0, 0, s_z, 0, // row3
          0, 0, 0, 1;   // row4

  return mat;
}

MatrixPtr create_rotation_mat(double r_x, double r_y, double r_z, double angle_in_rad) {
  double magnitude = sqrt(r_x*r_x + r_y*r_y + r_z*r_z);
  double u_x = r_x/magnitude;
  double u_y = r_y/magnitude;
  double u_z = r_z/magnitude;

  return create_rot_mat_helper(u_x, u_y, u_z, angle_in_rad);
}

MatrixPtr create_rot_mat_helper(double u_x, double u_y, double u_z, double angle_in_rad) {
  MatrixPtr mat = MatrixPtr(new Eigen::MatrixXd(4, 4));
  double m11 = u_x*u_x + (1 - u_x*u_x)*cos(angle_in_rad);
  double m12 = u_x*u_y*(1 - cos(angle_in_rad)) - u_z*sin(angle_in_rad);
  double m13 = u_x*u_z*(1 - cos(angle_in_rad)) + u_y*sin(angle_in_rad);
  double m21 = u_y*u_x*(1 - cos(angle_in_rad)) + u_z*sin(angle_in_rad);
  double m22 = u_y*u_y + (1 - u_y*u_y)*cos(angle_in_rad);
  double m23 = u_y*u_z*(1 - cos(angle_in_rad)) - u_x*sin(angle_in_rad);
  double m31 = u_z*u_x*(1 - cos(angle_in_rad)) - u_y*sin(angle_in_rad);
  double m32 = u_z*u_y*(1 - cos(angle_in_rad)) + u_x*sin(angle_in_rad);
  double m33 = u_z*u_z + (1 - u_z*u_z)*cos(angle_in_rad);

  *mat << m11, m12, m13, 0, // row1
          m21, m22, m23, 0, // row2
          m31, m32, m33, 0, // row3
          0, 0, 0, 1;   // row4

  return mat;
}

VertexPtr transform_vertex(MatrixPtr trans_mat, VertexPtr vertex) {
  MatrixPtr vertex_mat = MatrixPtr(new Eigen::MatrixXd(4, 1));
  *vertex_mat << vertex->x, // row1
                 vertex->y, // row2
                 vertex->z, // row3
                 1;         // row4

  Eigen::MatrixXd transformed = *trans_mat * *vertex_mat;

  double new_x = transformed(0) / transformed(3);
  double new_y = transformed(1) / transformed(3);
  double new_z = transformed(2) / transformed(3);

  return VertexPtr(new Vertex(new_x, new_y, new_z));
}

NormalPtr transform_normal(MatrixPtr trans_mat, NormalPtr normal) {
  MatrixPtr normal_mat = MatrixPtr(new Eigen::MatrixXd(4, 1));
  *normal_mat << normal->x, // row1
                 normal->y, // row2
                 normal->z, // row3
                 1;         // row4

  Eigen::MatrixXd transformed = *trans_mat * *normal_mat;

  double new_x = transformed(0) / transformed(3);
  double new_y = transformed(1) / transformed(3);
  double new_z = transformed(2) / transformed(3);

  return NormalPtr(new Normal(new_x, new_y, new_z));
}

VertexPtr scale_vertex(double factor, VertexPtr vertex) {
  double new_x = vertex->x * factor;
  double new_y = vertex->y * factor;
  double new_z = vertex->z * factor;

  return VertexPtr(new Vertex(new_x, new_y, new_z));
}
