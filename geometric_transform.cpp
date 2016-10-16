#include "geometric_transform.hpp"

#include <fstream> // basic file operations
#include <iostream>
#include <math.h>
#include <memory> // shared_ptr
#include <string>
#include <vector>

#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

shared_ptr<Eigen::MatrixXd> inverse_transform(vector<string> lines) {
  shared_ptr<Eigen::MatrixXd> prod = multiply_matrices(lines);
  shared_ptr<Eigen::MatrixXd> res = shared_ptr<Eigen::MatrixXd>(new Eigen::MatrixXd(4, 4));
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

shared_ptr<Eigen::MatrixXd> multiply_matrices(vector<string> lines) {
  shared_ptr<Eigen::MatrixXd> prod = shared_ptr<Eigen::MatrixXd>(new Eigen::MatrixXd(4, 4));
  prod->setIdentity(4, 4);

  for(vector<string>::iterator line_it = lines.begin(); line_it != lines.end(); ++line_it) {
    multiply_into_matrix(*line_it, prod);
  }

  return prod;
}

void multiply_into_matrix(string line, shared_ptr<Eigen::MatrixXd> mat) {
  shared_ptr<Eigen::MatrixXd> new_mat = parse_transform_line(line);
  *mat = *new_mat * *mat;
}

shared_ptr<Eigen::MatrixXd> parse_transform_line(string line) {
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

shared_ptr<Eigen::MatrixXd> parse_scaling_line(string line) {
  istringstream line_stream(line);
  char s;
  double s_x, s_y, s_z;
  if (!(line_stream >> s >> s_x >> s_y >> s_z)) {
    throw "Wrong number of arguments to scaling line";
  }

  return create_scaling_mat(s_x, s_y, s_z);
}

shared_ptr<Eigen::MatrixXd> parse_translation_line(string line) {
  istringstream line_stream(line);
  char t;
  double t_x, t_y, t_z;
  if (!(line_stream >> t >> t_x >> t_y >> t_z)) {
    throw "Wrong number of arguments to translation line";
  }

  return create_translation_mat(t_x, t_y, t_z);
}

shared_ptr<Eigen::MatrixXd> parse_rotation_line(string line) {
  istringstream line_stream(line);
  char r;
  double r_x, r_y, r_z, angle_in_rad;
  if (!(line_stream >> r >> r_x >> r_y >> r_z >> angle_in_rad)) {
    throw "Wrong number of arguments to translation line";
  }

  return create_rotation_mat(r_x, r_y, r_z, angle_in_rad);
}

shared_ptr<Eigen::MatrixXd> create_translation_mat(double t_x, double t_y, double t_z) {
  shared_ptr<Eigen::MatrixXd> mat = shared_ptr<Eigen::MatrixXd>(new Eigen::MatrixXd(4, 4));

  *mat << 1, 0, 0, t_x, // row1
          0, 1, 0, t_y, // row2
          0, 0, 1, t_z, // row3
          0, 0, 0, 1;   // row4

  return mat;
}

shared_ptr<Eigen::MatrixXd> create_scaling_mat(double s_x, double s_y, double s_z) {
  shared_ptr<Eigen::MatrixXd> mat = shared_ptr<Eigen::MatrixXd>(new Eigen::MatrixXd(4, 4));

  *mat << s_x, 0, 0, 0, // row1
          0, s_y, 0, 0, // row2
          0, 0, s_z, 0, // row3
          0, 0, 0, 1;   // row4

  return mat;
}

shared_ptr<Eigen::MatrixXd> create_rotation_mat(double r_x, double r_y, double r_z, double angle_in_rad) {
  if (r_x == 1) {
    return create_rx_mat(angle_in_rad);

  } else if (r_y == 1) {
    return create_ry_mat(angle_in_rad);

  } else if (r_z == 1) {
    return create_rz_mat(angle_in_rad);

  } else {
    throw "No axis specified for rotation";
  }
}

shared_ptr<Eigen::MatrixXd> create_rx_mat(double angle_in_rad) {
  shared_ptr<Eigen::MatrixXd> mat = shared_ptr<Eigen::MatrixXd>(new Eigen::MatrixXd(4, 4));

  *mat << 1, 0, 0, 0, // row1
          0, cos(angle_in_rad), -sin(angle_in_rad), 0, // row2
          0, sin(angle_in_rad), cos(angle_in_rad), 0, // row3
          0, 0, 0, 1;   // row4

  return mat;
}

shared_ptr<Eigen::MatrixXd> create_ry_mat(double angle_in_rad) {
  shared_ptr<Eigen::MatrixXd> mat = shared_ptr<Eigen::MatrixXd>(new Eigen::MatrixXd(4, 4));

  *mat << cos(angle_in_rad), 0, sin(angle_in_rad), 0, // row1
          0, 1, 0, 0, // row2
          -sin(angle_in_rad), 0, cos(angle_in_rad), 0, // row3
          0, 0, 0, 1;   // row4

  return mat;
}

shared_ptr<Eigen::MatrixXd> create_rz_mat(double angle_in_rad) {
  shared_ptr<Eigen::MatrixXd> mat = shared_ptr<Eigen::MatrixXd>(new Eigen::MatrixXd(4, 4));

  *mat << cos(angle_in_rad), -sin(angle_in_rad), 0, 0, // row1
          sin(angle_in_rad), cos(angle_in_rad), 0, 0, // row2
          0, 0, 1, 0, // row3
          0, 0, 0, 1;   // row4

  return mat;
}

shared_ptr<Vertex> transform_vertex(shared_ptr<Eigen::MatrixXd> trans_mat, shared_ptr<Vertex> vertex) {
  shared_ptr<Eigen::MatrixXd> vertex_mat = shared_ptr<Eigen::MatrixXd>(new Eigen::MatrixXd(4, 1));
  *vertex_mat << vertex->x, // row1
                 vertex->y, // row2
                 vertex->z, // row3
                 1;         // row4

  Eigen::MatrixXd transformed = *trans_mat * *vertex_mat;

  double new_x = transformed(0) / transformed(3);
  double new_y = transformed(1) / transformed(3);
  double new_z = transformed(2) / transformed(3);

  return shared_ptr<Vertex>(new Vertex(new_x, new_y, new_z));
}

shared_ptr<Vertex> scale_vertex(double factor, shared_ptr<Vertex> vertex) {
  double new_x = vertex->x * factor;
  double new_y = vertex->y * factor;
  double new_z = vertex->z * factor;

  return shared_ptr<Vertex>(new Vertex(new_x, new_y, new_z));
}
