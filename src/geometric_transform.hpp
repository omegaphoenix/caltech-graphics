#ifndef TRANSFORM_INVERSE_PROD_HPP
#define TRANSFORM_INVERSE_PROD_HPP

#include <memory> // shared_ptr
#include <vector>

#include "normal.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

using NormalPtr = shared_ptr<Normal>;
using MatrixPtr = shared_ptr<Eigen::MatrixXd>;
using VertexPtr = shared_ptr<Vertex>;

// Return the inverse transform from the vector of lines transformed to matrices
MatrixPtr inverse_transform(vector<string> lines);

// Reads file and returns input to inverse_transform
vector<string> parse_file_to_line_vector(char *file_name);

// Multiply multiple vectors after transforming to matrices
MatrixPtr multiply_matrices(vector<string> lines);
// Multiply multiple normals after transforming to matrices
MatrixPtr create_norm_trans_mat(vector<string> lines);

// Return line_mat times mat
void multiply_into_matrix(string line, MatrixPtr mat);

// Parse a single line
MatrixPtr parse_transform_line(string line);

// Helper functions for checking type of line
bool is_translation_line(string line);
bool is_rotation_line(string line);
bool is_scaling_line(string line);

// Helper functions for parsing types of lines
MatrixPtr parse_translation_line(string line);
MatrixPtr parse_scaling_line(string line);
MatrixPtr parse_rotation_line(string line);

// Creating respective matrices from inputs
MatrixPtr create_translation_mat(double t_x, double t_y, double t_z);
MatrixPtr create_scaling_mat(double s_x, double s_y, double s_z);
MatrixPtr create_rotation_mat(double r_x, double r_y, double r_z, double angle_in_rad);
MatrixPtr create_rot_mat_helper(double u_x, double u_y, double u_z, double angle_in_rad);

// Multiply vertex coordinates by matrix or scalar
VertexPtr transform_vertex(MatrixPtr trans_mat, VertexPtr vertex);
NormalPtr transform_normal(MatrixPtr trans_mat, NormalPtr vertex);
VertexPtr scale_vertex(double factor, VertexPtr vertex);

#endif
