#include "halfedge.hpp"

#include <algorithm>
#include <cassert>
#include <map>
#include <iostream>
#include <utility>
#include <vector>

#include "structs.hpp"

#include "Eigen/Dense"

using namespace std;

pair<int, int> get_edge_key(int x, int y) {
  assert(x != y);
  return pair<int, int>(min(x, y), max(x, y));
}

void hash_edge(map<pair<int, int>, HE*> &edge_hash,
    pair<int, int> edge_key,
    HE *edge) {
  if(edge_hash.count(edge_key) != 0) {
    HE *flip = edge_hash[edge_key];
    flip->flip = edge;
    edge->flip = flip;
  }
  else
    edge_hash[edge_key] = edge;
}

bool check_flip(HE *edge) {
  return edge->flip == NULL || edge->flip->vertex != edge->vertex;
}

bool check_edge(HE *edge) {
  return check_flip(edge) || !edge->face->oriented || !edge->flip->face->oriented;
}

bool check_face(HEF *face) {
  bool b1 = check_edge(face->edge);
  bool b2 = (face->edge->next != NULL) ? check_edge(face->edge->next) : 1;
  bool b3;

  if(face->edge->next != NULL) {
    b3 = (face->edge->next->next != NULL) ? check_edge(face->edge->next->next) : 1;
  } else {
    b3 = 1;
  }

  return b1 && b2 && b3;
}

bool orient_flip_face(HE *edge) {
  if(edge->flip == NULL) {
    return 1;
  }

  HE *flip = edge->flip;
  HEF *face = flip->face;

  if(face->oriented) {
    return check_face(face);
  }

  if (!check_flip(edge)) {
    HEV *v1 = face->edge->vertex;
    HEV *v2 = face->edge->next->vertex;
    HEV *v3 = face->edge->next->next->vertex;

    assert(v1 != v2 && v1 != v3 && v2 != v3);

    HE *e3 = face->edge;
    HE *e1 = face->edge->next;
    HE *e2 = face->edge->next->next;

    assert(e3->vertex == v1);
    assert(e1->vertex == v2);
    assert(e2->vertex == v3);

    e3->vertex = v3;
    e3->next = e2;

    e1->vertex = v1;
    e1->next = e3;

    e2->vertex = v2;
    e2->next = e1;

    v1->out = e3;
    v2->out = e1;
    v3->out = e2;

    assert(face->edge->next->next->next == face->edge);
  }

  face->oriented = 1;

  assert(check_flip(edge));
  assert(check_face(face));

  return check_face(face) && orient_face(face);
}

bool orient_face(HEF *face) {
  assert(face->oriented);
  return orient_flip_face(face->edge)
    && orient_flip_face(face->edge->next)
    && orient_flip_face(face->edge->next->next)
    && check_face(face);
}

bool build_HE(Mesh_Data *mesh, vector<HEV *> *hevs, vector<HEF *> *hefs) {
  vector<Vertex> *vertices = mesh->vertices;
  vector<Face> *faces = mesh->faces;

  hevs->push_back(NULL);
  map<pair<int, int>, HE*> edge_hash;

  int size_vertices = vertices->size();

  for(int i = 1; i < size_vertices; ++i) {
    HEV *hev = new HEV;
    hev->x = vertices->at(i).x;
    hev->y = vertices->at(i).y;
    hev->z = vertices->at(i).z;
    hev->out = NULL;
    hev->index = i;

    hevs->push_back(hev);
  }

  HEF *first_face = NULL;
  int num_faces = faces->size();

  for (int i = 0; i < num_faces; ++i) {
    Face f = faces->at(i);

    HE *e1 = new HE;
    HE *e2 = new HE;
    HE *e3 = new HE;

    e1->flip = NULL;
    e2->flip = NULL;
    e3->flip = NULL;

    HEF *hef = new HEF;

    hef->oriented = 0;
    hef->edge = e1;

    e1->face = hef;
    e2->face = hef;
    e3->face = hef;

    e1->next = e2;
    e2->next = e3;
    e3->next = e1;

    e1->vertex = hevs->at(f.vertex1);
    e2->vertex = hevs->at(f.vertex2);
    e3->vertex = hevs->at(f.vertex3);

    hevs->at(f.vertex1)->out = e1;
    hevs->at(f.vertex2)->out = e2;
    hevs->at(f.vertex3)->out = e3;

    hash_edge(edge_hash, get_edge_key(f.vertex1, f.vertex2), e1);
    hash_edge(edge_hash, get_edge_key(f.vertex2, f.vertex3), e2);
    hash_edge(edge_hash, get_edge_key(f.vertex3, f.vertex1), e3);

    hefs->push_back(hef);

    if(first_face == NULL) {
      first_face = hef;
      first_face->oriented = 1;
    }
  }

  return orient_face(first_face);
}

void delete_HE(vector<HEV*> *hevs, vector<HEF*> *hefs) {
  int hev_size = hevs->size();
  int num_hefs = hefs->size();

  for(int i = 1; i < hev_size; ++i) {
    delete hevs->at(i);
  }

  for(int i = 0; i < num_hefs; ++i) {
    delete hefs->at(i)->edge->next->next;
    delete hefs->at(i)->edge->next;
    delete hefs->at(i)->edge;
    delete hefs->at(i);
  }

  delete hevs;
  delete hefs;
}

Eigen::Vector3d HEV_to_vec(HEV* vertex) {
  return Eigen::Vector3d(vertex->x, vertex->y, vertex->z);
}

Eigen::Vector3d HEV_to_vec(HEV* v1, HEV* v2) {
  return Eigen::Vector3d(v2->x - v1->x, v2->y - v1->y, v2->z - v1->z);
}

Eigen::Vector3d calc_normal(HEF* face) {
  Eigen::Vector3d v1 = HEV_to_vec(face->edge->vertex);
  Eigen::Vector3d v2 = HEV_to_vec(face->edge->next->vertex);
  Eigen::Vector3d v3 = HEV_to_vec(face->edge->next->next->vertex);

  return (v2 - v1).cross(v3 - v1);
}

double calc_area(Eigen::Vector3d normal) {
  return normal.norm();
}

Normal calc_vertex_normal(HEV *vertex) {
  float x = 0;
  float y = 0;
  float z = 0;

  HE *he = vertex->out; // get outgoing halfedge from given vertex

  do {
    // compute the normal of the plane of the face
    Eigen::Vector3d face_normal = calc_normal(he->face);
    // compute the area of the triangular face
    double face_area = calc_area(face_normal);

    // accummulate onto our normal vector
    x += face_normal(0) * face_area;
    y += face_normal(1) * face_area;
    z += face_normal(2) * face_area;

    // gives us the halfedge to the next adjacent vertex
    he = he->flip->next;
  }
  while(he != vertex->out);

  return Normal(x, y, z);
}
