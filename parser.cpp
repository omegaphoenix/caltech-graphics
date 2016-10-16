#include "parser.hpp"

#include <fstream> // basic file operations
#include <iostream>
#include <map>
#include <memory> // shared_ptr
#include <sstream>
#include <string>
#include <vector>

#include "camera.hpp"
#include "face.hpp" // represents triangular face
#include "three_d_model.hpp" // .obj file data stored in model
#include "three_d_model_transform.hpp"
#include "vertex.hpp" // represents 3d coordinate

using namespace std;

shared_ptr<Camera> get_camera_data(ifstream& obj_transform_file) {
  vector<string> lines;
  string line;

  getline(obj_transform_file, line);
  while (line != "") {
    lines.push_back(line);
    getline(obj_transform_file, line);
  }

  shared_ptr<Camera> cam = shared_ptr<Camera>(new Camera(lines));
  return cam;
}

shared_ptr<vector<shared_ptr<ThreeDModel>>> store_file_objects(int argc, char **argv) {
  shared_ptr<vector<shared_ptr<ThreeDModel>>> models = shared_ptr<vector<shared_ptr<ThreeDModel>>>(new vector<shared_ptr<ThreeDModel> >());
  for (int i = 1; i < argc; i++) {
    shared_ptr<ThreeDModel> model = parse_file_to_model(string(argv[i]));
    models->push_back(model);
  }
  return models;
}

shared_ptr<ThreeDModel> parse_file_to_model(string file_name) {
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

shared_ptr<map<string, shared_ptr<ThreeDModelTransform>>> get_objects(ifstream& obj_transform_file, shared_ptr<Camera> cam) {
  shared_ptr<map<string, shared_ptr<ThreeDModelTransform>>> models = shared_ptr<map<string, shared_ptr<ThreeDModelTransform>>>(new map<string, shared_ptr<ThreeDModelTransform> >());

  string line;
  getline(obj_transform_file, line);
  getline(obj_transform_file, line);
  while (line != "") {
    create_obj(line, models, cam);
    getline(obj_transform_file, line);
  }

  return models;
}

void create_obj(string line, shared_ptr<map<string, shared_ptr<ThreeDModelTransform>>> models, shared_ptr<Camera> cam) {
  istringstream line_stream(line);

  string obj_name, obj_filename;
  if (!(line_stream >> obj_name >> obj_filename)) {
    throw "Wrong number of arguments to object line";
  }

  obj_filename = "data/" + obj_filename;

  shared_ptr<ThreeDModelTransform> new_model = create_model(obj_name, obj_filename);
  new_model->cam = cam;
  (*models)[obj_name] = new_model;
}

shared_ptr<ThreeDModelTransform> create_model(string obj_name, string obj_filename) {
  shared_ptr<ThreeDModelTransform> transform_model = shared_ptr<ThreeDModelTransform>(new ThreeDModelTransform());

  transform_model->model = parse_file_to_model(obj_filename.c_str());
  transform_model->copy_num = 0;
  transform_model->name = obj_name;

  return transform_model;
}
