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
#include "light.hpp"
#include "normal.hpp"
#include "three_d_model.hpp" // .obj file data stored in model
#include "three_d_model_transform.hpp"
#include "vertex.hpp" // represents 3d coordinate

using namespace std;

using CameraPtr = shared_ptr<Camera>;
using LightPtr = shared_ptr<Light>;
using NormalPtr = shared_ptr<Normal>;
using ReflectPtr = shared_ptr<Reflectance>;
using ThreeDModelPtr = shared_ptr<ThreeDModel>;
using ThreeDModelTransformPtr = shared_ptr<ThreeDModelTransform>;
using VertexPtr = shared_ptr<Vertex>;

using LightVecPtr = shared_ptr<vector<LightPtr>>;

CameraPtr parse_camera_data(char *file_name) {
  ifstream obj_transform_file(file_name);
  return get_camera_data(obj_transform_file);
}

LightVecPtr parse_light_data(char *file_name) {
  ifstream obj_transform_file(file_name);
  get_camera_data(obj_transform_file);
  return get_light_data(obj_transform_file);
}

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

LightVecPtr get_light_data(ifstream& obj_transform_file) {
  LightVecPtr lights = LightVecPtr(new vector<LightPtr>());
  string line;

  getline(obj_transform_file, line);
  while (line != "") {
    store_light_line(line, lights);
    getline(obj_transform_file, line);
  }

  return lights;
}

void store_light_line(string line, LightVecPtr lights) {
  istringstream line_stream(line);
  string light;
  char comma1, comma2;
  double x, y, z, r, g, b, att;
  if (!(line_stream >> light >> x >> y >> z >> comma1 >> r >> g >> b >> comma2 >>att)) {
    throw "Wrong number of arguments to vertex line";
  }

  lights->push_back(LightPtr(new Light(x, y, z, r, g, b, att)));
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
  ThreeDModelPtr model = ThreeDModelPtr(new ThreeDModel(file_name));

  string line;
  while (getline(obj_file, line)) {
    store_obj_line(line, model);
  }

  return model;
}

void store_material_properties(vector<string> lines, ThreeDModelPtr model) {
  store_ambient_prop(lines, model);
  store_diffuse_prop(lines, model);
  store_specular_prop(lines, model);
  store_shininess_prop(lines, model);
}

void store_ambient_prop(vector<string> lines, ThreeDModelPtr model) {
  string line = lines.front();;
  lines.erase(lines.begin());

  istringstream line_stream(line);
  string _;
  double r, g, b;
  if (!(line_stream >> _ >> r >> g >> b)) {
    throw "Wrong number of arguments to diffuse line";
  }

  model->material->ambient = ReflectPtr(new Reflectance(r, g, b));
}

void store_diffuse_prop(vector<string> lines, ThreeDModelPtr model) {
  string line = lines.front();;
  lines.erase(lines.begin());

  istringstream line_stream(line);
  string _;
  double r, g, b;
  if (!(line_stream >> _ >> r >> g >> b)) {
    throw "Wrong number of arguments to diffuse line";
  }

  model->material->diffuse = ReflectPtr(new Reflectance(r, g, b));
}

void store_specular_prop(vector<string> lines, ThreeDModelPtr model) {
  string line = lines.front();;
  lines.erase(lines.begin());

  istringstream line_stream(line);
  string _;
  double r, g, b;
  if (!(line_stream >> _ >> r >> g >> b)) {
    throw "Wrong number of arguments to specular line";
  }

  model->material->specular = ReflectPtr(new Reflectance(r, g, b));
}

void store_shininess_prop(vector<string> lines, ThreeDModelPtr model) {
  string line = lines.front();;
  lines.erase(lines.begin());

  istringstream line_stream(line);
  string _;
  double shiny;
  if (!(line_stream >> _ >> shiny)) {
    throw "Wrong number of arguments to shininess line";
  }

  model->material->shininess = shiny;
}

void store_obj_line(string line, ThreeDModelPtr model) {
  if (is_vertex_line(line)) {
    store_vertex_line(line, model);

  } else if (is_normal_line(line)) {
    store_normal_line(line, model);

  } else if (is_face_line(line)) {
    store_face_line(line, model);
  }
}

bool is_vertex_line(string line) {
  return line.at(0) == 'v' && line.at(1) == ' ';
}

bool is_normal_line(string line) {
  return line.at(0) == 'v' && line.at(1) == 'n';
}

bool is_face_line(string line) {
  return line.at(0) == 'f';
}

void store_vertex_line(string line, ThreeDModelPtr model) {
  istringstream line_stream(line);
  char _;
  double x, y, z;
  if (!(line_stream >> _ >> x >> y >> z)) {
    throw "Wrong number of arguments to vertex line";
  }

  model->vertices->push_back(VertexPtr(new Vertex(x, y, z)));
}

void store_normal_line(string line, ThreeDModelPtr model) {
  istringstream line_stream(line);
  string _;
  double x, y, z;
  if (!(line_stream >> _ >> x >> y >> z)) {
    throw "Wrong number of arguments to normal line";
  }

  model->normals->push_back(NormalPtr(new Normal(x, y, z)));
}

void store_face_line(string line, ThreeDModelPtr model) {
  istringstream line_stream(line);

  char _;
  string vn1, vn2, vn3;

  if (!(line_stream >> _ >> vn1 >> vn2 >> vn3)) {
    throw "Wrong number of arguments to face line";
  }

  string delimiter = "//";

  size_t pos = vn1.find(delimiter);
  int v1 = atoi(vn1.substr(0, pos).c_str());
  vn1.erase(0, pos + delimiter.length());
  int n1 = atoi(vn1.c_str());

  pos = vn2.find(delimiter);
  int v2 = atoi(vn2.substr(0, pos).c_str());
  vn2.erase(0, pos + delimiter.length());
  int n2 = atoi(vn2.c_str());

  pos = vn3.find(delimiter);
  int v3 = atoi(vn3.substr(0, pos).c_str());
  vn3.erase(0, pos + delimiter.length());
  int n3 = atoi(vn3.c_str());


  model->faces->push_back(shared_ptr<Face>(new Face (v1, v2, v3, n1, n2, n3)));
}

shared_ptr<map<string, ThreeDModelTransformPtr>> get_objects(ifstream& obj_transform_file, CameraPtr cam) {
  shared_ptr<map<string, ThreeDModelTransformPtr>> models =
    shared_ptr<map<string, ThreeDModelTransformPtr>>(new map<string, ThreeDModelTransformPtr>());

  string line;
  getline(obj_transform_file, line); // Ignore first line
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
