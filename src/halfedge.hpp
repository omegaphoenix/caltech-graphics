// Adapted from code by Kevin (Kevli) Li (Class of 2016)

#ifndef HALFEDGE_HPP
#define HALFEDGE_HPP

#include <algorithm>
#include <cassert>
#include <map>
#include <iostream>
#include <utility>
#include <vector>

#include "structs.hpp"

#include "Eigen/Dense"

using namespace std;

 /* Halfedge structs */

struct HE { // HE for halfedge
  struct HEV *vertex;
  // the face adjacent to this halfedge
  struct HEF *face;
  struct HE *flip, *next;
};

struct HEF { // HEF for halfedge face
  struct HE *edge;
  // this variable is used to help orientate the halfedge when building it;
  bool oriented;
};

struct HEV { // HEV for halfedge vertex
  double x, y, z;
  struct HE *out;
  // use this to store your index for this vertex when you index the vertices
  // before building the operator for implicit fairing
  int index;
};

/* Function prototypes */

pair<int, int> get_edge_key(int x, int y);
void hash_edge(map<pair<int, int>, HE*> &edge_hash,
    pair<int, int> edge_key,
    HE *edge);

bool check_flip(HE *edge);
bool check_edge(HE *edge);
bool check_face(HEF *face);

bool orient_flip_face(HE *edge);
bool orient_face(HEF *face);

// Populate and delete halfedge vectors
bool build_HE(Mesh_Data *mesh, vector<HEV *> *hevs, vector<HEF *> *hefs);
void delete_HE(vector<HEV*> *hevs, vector<HEF*> *hefs);

// Convert to vectors
Eigen::Vector3d HEV_to_vec(HEV* vertex);
Eigen::Vector3d HEV_to_vec(HEV* v1, HEV* v2);

// Calculate the normal and use the normal to calculate area
Eigen::Vector3d calc_normal(HEF* face);
// area = norm of normal
double calc_area(Eigen::Vector3d normal);

// Calculate the normal of the vertex based on the halfedge
Normal calc_vertex_normal(HEV *vertex);

#endif
