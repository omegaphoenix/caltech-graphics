#ifndef TRANSFORM_INVERSE_PROD_HPP
#define TRANSFORM_INVERSE_PROD_HPP

#include <vector>

#include "Eigen/Dense"

using namespace std;

// Return the inverse transform from the vector of lines transformed to matrices
Eigen::MatrixXd *inverse_transform(vector<string> *lines);

// Reads file and returns input to inverse_transform
vector<string> *parse_file_to_line_vector(char *file_name);

// Multiply multiple vectors after transforming to matrices
Eigen::MatrixXd *multiply_matrices(vector<string> *lines);

// Return line_mat times mat
void multiply_into_matrix(string line, Eigen::MatrixXd *mat);

// Parse a single line
Eigen::MatrixXd *parse_transform_line(string line);

// Helper functions for checking type of line
bool is_translation_line(string line);
bool is_rotation_line(string line);
bool is_scaling_line(string line);

// Helper functions for parsing types of lines
Eigen::MatrixXd *parse_translation_line(string line);
Eigen::MatrixXd *parse_scaling_line(string line);
Eigen::MatrixXd *parse_rotation_line(string line);

// Creating respective matrices from inputs
Eigen::MatrixXd *create_translation_mat(double t_x, double t_y, double t_z);
Eigen::MatrixXd *create_scaling_mat(double s_x, double s_y, double s_z);
Eigen::MatrixXd *create_rotation_mat(double r_x, double r_y, double r_z, double angle_in_rad);

// Helper functions for create_rotation_mat
Eigen::MatrixXd *create_rx_mat(double angle_in_rad);
Eigen::MatrixXd *create_ry_mat(double angle_in_rad);
Eigen::MatrixXd *create_rz_mat(double angle_in_rad);

#endif
