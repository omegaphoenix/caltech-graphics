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

using CameraPtr = shared_ptr<Camera>;
using ThreeDModelPtr = shared_ptr<ThreeDModel>;
using ThreeDModelTransformPtr = shared_ptr<ThreeDModelTransform>;
using VertexPtr = shared_ptr<Vertex>;

CameraPtr get_camera_data(ifstream& obj_transform_file) {
  vector<string> lines;
  string line;

  getline(obj_transform_file, line);
  while (line != "") {
    lines.push_back(line);
    getline(obj_transform_file, line);
  }

  CameraPtr cam = CameraPtr(new Camera(lines));
  return cam;
}

shared_ptr<vector<ThreeDModelPtr>> store_file_objects(int argc, char **argv) {
  shared_ptr<vector<ThreeDModelPtr>> models =
    shared_ptr<vector<ThreeDModelPtr>>(new vector<ThreeDModelPtr>());
  for (int i = 1; i < argc; i++) {
    ThreeDModelPtr model = parse_file_to_model(string(argv[i]));
    models->push_back(model);
  }
  return models;
}

ThreeDModelPtr parse_file_to_model(string file_name) {
  ifstream obj_file(file_name);

  ThreeDModelPtr model =
    ThreeDModelPtr(new ThreeDModel(file_name));
  string line;
  while (getline(obj_file, line)) {
    store_line(line, model);
  }

  return model;
}

void store_line(string line, ThreeDModelPtr model) {
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

void store_vertex_line(string line, ThreeDModelPtr model) {
  istringstream line_stream(line);
  char v;
  double x, y, z;
  if (!(line_stream >> v >> x >> y >> z)) {
    throw "Wrong number of arguments to vertex line";
  }

  model->vertices->push_back(VertexPtr(new Vertex(x, y, z)));
}

void store_face_line(string line, ThreeDModelPtr model) {
  istringstream line_stream(line);
  char f;
  int v1, v2, v3;
  if (!(line_stream >> f >> v1 >> v2 >> v3)) {
    throw "Wrong number of arguments to face line";
  }

  model->faces->push_back(shared_ptr<Face>(new Face (v1, v2, v3)));
}

shared_ptr<map<string, ThreeDModelTransformPtr>> get_objects(ifstream& obj_transform_file, CameraPtr cam) {
  shared_ptr<map<string, ThreeDModelTransformPtr>> models =
    shared_ptr<map<string, ThreeDModelTransformPtr>>(new map<string, ThreeDModelTransformPtr >());

  string line;
  getline(obj_transform_file, line);
  getline(obj_transform_file, line);
  while (line != "") {
    create_obj(line, models, cam);
    getline(obj_transform_file, line);
  }

  return models;
}

void create_obj(string line, shared_ptr<map<string, ThreeDModelTransformPtr>> models, CameraPtr cam) {
  istringstream line_stream(line);

  string obj_name, obj_filename;
  if (!(line_stream >> obj_name >> obj_filename)) {
    throw "Wrong number of arguments to object line";
  }

  obj_filename = "data/" + obj_filename;

  ThreeDModelTransformPtr new_model = create_model(obj_name, obj_filename);
  new_model->cam = cam;
  (*models)[obj_name] = new_model;
}

ThreeDModelTransformPtr create_model(string obj_name, string obj_filename) {
  ThreeDModelTransformPtr transform_model =
    ThreeDModelTransformPtr(new ThreeDModelTransform());

  transform_model->model = parse_file_to_model(obj_filename.c_str());
  transform_model->copy_num = 0;
  transform_model->name = obj_name;

  return transform_model;
}
