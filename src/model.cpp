#include "model.hpp"

#include <cstddef>
#include <iostream>
#include <memory> // shared_ptr
#include <string>
#include <vector>

#include "halfedge.hpp"
#include "structs.hpp"

using namespace std;

using MaterialPtr = shared_ptr<Material>;

// empty constructor
Model :: Model() {
  name = "";
  setup_vertices();
  faces = vector<Face>();
  material = MaterialPtr(new Material());
}

// constructor using file
Model :: Model(string raw_file_name) {
  name = get_name(raw_file_name);
  setup_vertices();
  faces = vector<Face>();
  material = MaterialPtr(new Material());
}

// helper function for constructor to get object name
string Model :: get_name(string raw_file_name) {
  // Remove .obj from name
  return raw_file_name.substr(0, raw_file_name.size() - 4);
}

// helper function for constructor to get vertices
void Model :: setup_vertices() {
  vertices = vector<Vertex>();
  // Index 0 is filler because vertices are 1-indexed
  vertices.push_back(Vertex(0, 0, 0));
}

void Model :: set_variables() {
  Mesh_Data *mesh_data = new Mesh_Data;
  mesh_data->vertices = &vertices;
  mesh_data->faces = &faces;

  vector<HEV *> *hevs = new vector<HEV *>();
  vector<HEF *> *hefs = new vector<HEF *>();
  build_HE(mesh_data, hevs, hefs);


  HE* half_edge;
  for (vector<HEF*>::iterator face_it = hefs->begin(); face_it != hefs->end(); face_it++) {
    half_edge = (*face_it)->edge;
    vertex_buffer.push_back(vertices[half_edge->vertex->index]);
    normal_buffer.push_back(calc_vertex_normal(half_edge->vertex));

    for (int i = 0; i < 2; i++) {
      half_edge = half_edge->next;
      vertex_buffer.push_back(vertices[half_edge->vertex->index]);
      normal_buffer.push_back(calc_vertex_normal(half_edge->vertex));
    }
  }

  ambient_reflect[0] = material->ambient->red;
  ambient_reflect[1] = material->ambient->green;
  ambient_reflect[2] = material->ambient->blue;

  diffuse_reflect[0] = material->diffuse->red;
  diffuse_reflect[1] = material->diffuse->green;
  diffuse_reflect[2] = material->diffuse->blue;

  specular_reflect[0] = material->specular->red;
  specular_reflect[1] = material->specular->green;
  specular_reflect[2] = material->specular->blue;

  shininess = material->shininess;

  delete mesh_data;
  delete_HE(hevs, hefs);
}
