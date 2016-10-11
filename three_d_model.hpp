#ifndef THREE_D_MODEL_HPP
#define THREE_D_MODEL_HPP

#include <string>
#include <vector>

#include "face.hpp"
#include "vertex.hpp"

using namespace std;

struct ThreeDModel {
  string name;
  vector<Vertex *> *vertices;
  vector<Face *> *faces;

  // empty constructor
  ThreeDModel() {
  }

  // constructor using file
  ThreeDModel(char *raw_file_name) {
    name = get_name(raw_file_name);
    setup_vertices();
    faces = new vector<Face *>();
  }

  // helper function for constructor to get object name
  string get_name(char *raw_file_name) {
    // Remove .obj from name
    string str_name(raw_file_name);
    return str_name.substr(0, str_name.size() - 4);
  }

  // helper function for constructor to get vertices
  void setup_vertices() {
    vertices = new vector<Vertex *>();
    // Index 0 is NULL because vertices are 1-indexed
    vertices->push_back(NULL);
  }
};

#endif
