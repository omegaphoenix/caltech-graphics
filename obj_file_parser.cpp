#include "obj_file_parser.hpp"

#include <fstream> // basic file operations
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "face.hpp" // represents triangular face
#include "three_d_model.hpp" // .obj file data stored in model
#include "vertex.hpp" // represents 3d coordinate

using namespace std;

shared_ptr<vector<shared_ptr<ThreeDModel>>> store_file_objects(int argc, char **argv) {
  shared_ptr<vector<shared_ptr<ThreeDModel>>> models = shared_ptr<vector<shared_ptr<ThreeDModel>>>(new vector<shared_ptr<ThreeDModel> >());
  for (int i = 1; i < argc; i++) {
    shared_ptr<ThreeDModel> model = parse_file_to_model(argv[i]);
    models->push_back(model);
  }
  return models;
}

shared_ptr<ThreeDModel> parse_file_to_model(char *file_name) {
  ifstream obj_file(file_name);

  shared_ptr<ThreeDModel> model = shared_ptr<ThreeDModel>(new ThreeDModel(file_name));
  string line;
  while (getline(obj_file, line)) {
    store_line(line, model);
  }

  return model;
}

void store_line(string line, shared_ptr<ThreeDModel> model) {
  if (is_vertex_line(line)) {
    store_vertex_line(line, model);

  } else if (is_face_line(line)) {
    store_face_line(line, model);
  }
}

bool is_vertex_line(string line) {
  return line.at(0) == 'v';
}

bool is_face_line(string line) {
  return line.at(0) == 'f';
}

void store_vertex_line(string line, shared_ptr<ThreeDModel> model) {
  istringstream line_stream(line);
  char v;
  double x, y, z;
  if (!(line_stream >> v >> x >> y >> z)) {
    throw "Wrong number of arguments to vertex line";
  }

  model->vertices->push_back(shared_ptr<Vertex>(new Vertex(x, y, z)));
}

void store_face_line(string line, shared_ptr<ThreeDModel> model) {
  istringstream line_stream(line);
  char f;
  int v1, v2, v3;
  if (!(line_stream >> f >> v1 >> v2 >> v3)) {
    throw "Wrong number of arguments to face line";
  }

  model->faces->push_back(shared_ptr<Face>(new Face (v1, v2, v3)));
}
