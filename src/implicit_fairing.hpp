#ifndef IMPLICIT_FAIRING_HPP
#define IMPLICIT_FAIRING_HPP

#include <vector>

#include "halfedge.hpp"
#include "model.hpp"
#include "structs.hpp"

#include <Eigen/Dense>
#include <Eigen/Sparse>

using namespace std;

const double EPSILON = 0.000001;

// function to assign each vertex in our mesh to an index
void index_vertices(vector<HEV *> *vertices);

// Return cot(alpha_j) + cot(beta_j) using:
// cot = cos / sin = A dot B / |A cross B|
double cot_alpha_beta(HE *he);

/* Function to construct our Laplacian operator in matrix form:
 * (Delta x)_i = (1/(2A)) sum((cot(alpha_j) + cot(beta_j))x_j)
 *               - x_i sum(cot(alpha_j) + cot(beta_j))
 * Each row of the Laplacian matrix represents a single x_i.
 * Thus, each term in the row to be multiplied by x_j is
 * (1/(2A))(cot(alpha_j) + cot(beta_j)) except for column i which is
 * the sum of all the other columns in the row
 *
 * While multiplying the Laplacian terms by (1/2A) we also
 * multiply by -h and add 1 to the diagonal terms
 */
Eigen::SparseMatrix<double> build_F_operator(vector<HEV *> *vertices,
    double time_step);

// Solve for x_h in (I - h Delta) x_h = x_0
Eigen::VectorXd solve_x(Eigen::SparseMatrix<double> F, Model *model, vector<HEV *> *vertices, double time_step);

// Solve for y_h in (I - h Delta) y_h = y_0
Eigen::VectorXd solve_y(Eigen::SparseMatrix<double> F, Model *model, vector<HEV *> *vertices, double time_step);

// Solve for z_h in (I - h Delta) z_h = z_0
Eigen::VectorXd solve_z(Eigen::SparseMatrix<double> F, Model *model, vector<HEV *> *vertices, double time_step);

void update_vertices(Model *model, Eigen::VectorXd &xh, Eigen::VectorXd &yh, Eigen::VectorXd &zh);

void implicit_fairing(vector<Model> &objects, double time_step);

#endif
