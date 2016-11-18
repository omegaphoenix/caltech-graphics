#include "implicit_fairing.hpp"

#include <iostream>
#include <vector>

#include "halfedge.hpp"
#include "model.hpp"
#include "structs.hpp"

#include <Eigen/Dense>
#include <Eigen/Sparse>

using namespace std;

// function to assign each vertex in our mesh to an index
void index_vertices(vector<HEV *> *vertices) {
	// start at 1 because obj files are 1-indexed
	for(int i = 1; i < vertices->size(); ++i) {
		vertices->at(i)->index = i; // assign each vertex an index
	}
}

// Return cot(alpha_j) + cot(beta_j) using:
// cot = cos / sin = A dot B / |A cross B|
double cot_alpha_beta(HE *he) {
  HEV *hev1 = he->vertex;
  HEV *hev2 = he->next->vertex;
  HEV *hev3 = he->next->next->vertex;
  HEV *hev4 = he->flip->next->next->vertex;

  Eigen::Vector3d alpha_A = HEV_to_vec(hev3, hev1);
  Eigen::Vector3d alpha_B = HEV_to_vec(hev3, hev2);
  Eigen::Vector3d beta_A = HEV_to_vec(hev4, hev1);
  Eigen::Vector3d beta_B = HEV_to_vec(hev4, hev2);

  double cot_alpha_denom = (alpha_A.cross(alpha_B)).norm();
  double cot_alpha = alpha_A.dot(alpha_B) / cot_alpha_denom;

  double cot_beta_denom = (beta_A.cross(beta_B)).norm();
  double cot_beta = beta_A.dot(beta_B) / cot_beta_denom;

  double cot_sum = cot_alpha + cot_beta;
  return cot_sum;
}

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
    double time_step) {
  index_vertices(vertices); // assign each vertex an index

  // recall due to 1-indexing of obj files, index 0 doesn't contain a vertex
  int num_vertices = vertices->size() - 1;

  // initialize a sparse matrix to represent our F operator
  Eigen::SparseMatrix<double> F(num_vertices, num_vertices);

  // reserve room for 7 non-zeros per row of F
  F.reserve(Eigen::VectorXi::Constant(num_vertices, 7));

  for (int i = 1; i < vertices->size(); ++i) {
    HE *he = vertices->at(i)->out;
    double neighbor_area = 0;
    double cot_i = 0;

    // iterate over all vertices adjacent to v_i
    do {
      Eigen::Vector3d face_normal = calc_normal(he->face);
      double face_area = calc_area(face_normal);
      neighbor_area += face_area;
      // get index of adjacent vertex to v_i
      int j = he->next->vertex->index;

      // call function to compute cot(alpha_j) + cot(beta_j)
      double cot_j = cot_alpha_beta(he);
      cot_i += cot_j;

      // fill the j-th slot of row i of our Laplacian with appropriate value
      F.insert(i-1, j-1) = cot_j;

      he = he->flip->next;
    }
    while(he != vertices->at(i)->out);

    if (neighbor_area > EPSILON) {
      F.insert(i-1, i-1) = 1.0 + time_step/2.0/neighbor_area*cot_i;
      do {
        // get index of adjacent vertex to v_i
        int j = he->next->vertex->index;
        // multiply by -h/(2A)
        F.coeffRef(i-1, j-1) *= -1.0*time_step/2.0/neighbor_area;
        he = he->flip->next;
      }
      while(he != vertices->at(i)->out);
    }
  }

  F.makeCompressed(); // optional; tells Eigen to more efficiently store our sparse matrix
  return F;
}

// Solve for x_h in (I - h Delta) x_h = x_0
Eigen::VectorXd solve_x(Eigen::SparseMatrix<double> F, Model *model, double time_step) {
  vector<HEV *> *vertices = model->hevs;

  Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int> > solver;
  solver.analyzePattern(F);
  solver.factorize(F);

  int num_vertices = vertices->size() - 1;

  // initialize our vector representation of x_0
  Eigen::VectorXd x_0(num_vertices);
  for(int i = 1; i < vertices->size(); ++i) {
    x_0(i-1) = model->vertices.at(i).x;
  }

  Eigen::VectorXd x_h(num_vertices);
  x_h = solver.solve(x_0);
  return x_h;
}

// Solve for y_h in (I - h Delta) y_h = y_0
Eigen::VectorXd solve_y(Eigen::SparseMatrix<double> F, Model *model, double time_step) {
  vector<HEV *> *vertices = model->hevs;

  Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int> > solver;
  solver.analyzePattern(F);
  solver.factorize(F);

  int num_vertices = vertices->size() - 1;

  // initialize our vector representation of y_0
  Eigen::VectorXd y_0(num_vertices);
  for(int i = 1; i < vertices->size(); ++i) {
    y_0(i-1) = model->vertices.at(i).y;
  }

  Eigen::VectorXd y_h(num_vertices);
  y_h = solver.solve(y_0);
  return y_h;
}

// Solve for z_h in (I - h Delta) z_h = z_0
Eigen::VectorXd solve_z(Eigen::SparseMatrix<double> F, Model *model, double time_step) {
  vector<HEV *> *vertices = model->hevs;

  Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int> > solver;
  solver.analyzePattern(F);
  solver.factorize(F);

  int num_vertices = vertices->size() - 1;

  // initialize our vector representation of z_0
  Eigen::VectorXd z_0(num_vertices);
  for(int i = 1; i < vertices->size(); ++i) {
    z_0(i-1) = model->vertices.at(i).z;
  }

  Eigen::VectorXd z_h(num_vertices);
  z_h = solver.solve(z_0);
  return z_h;
}

void update_vertices(Model *model, Eigen::VectorXd &xh, Eigen::VectorXd &yh, Eigen::VectorXd &zh) {
  model->vertex_buffer.clear();
  model->normal_buffer.clear();

  int num_vertices = model->vertices.size();
  model->setup_vertices();
  for(int i = 1; i < num_vertices; ++i) {
    model->vertices.push_back(Vertex(xh(i-1), yh(i-1), zh(i-1)));
  }
  model->set_variables();
}

void implicit_fairing(vector<Model> &objects, double time_step) {
  for (vector<Model>::iterator obj_it = objects.begin(); obj_it != objects.end(); ++obj_it) {
    Model *model = &(*obj_it);
    Eigen::SparseMatrix<double> F = build_F_operator(model->hevs, time_step);

    // Calculate new vertices
    Eigen::VectorXd xh = solve_x(F, model, time_step);
    Eigen::VectorXd yh = solve_y(F, model, time_step);
    Eigen::VectorXd zh = solve_z(F, model, time_step);

    update_vertices(model, xh, yh, zh);
  }
}
