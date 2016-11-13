#include "parser.hpp"

#include <fstream> // basic file operations
#include <iostream>
#include <map>
#include <memory> // shared_ptr
#include <sstream>
#include <string>
#include <vector>

#include "camera.hpp"
#include "model.hpp" // .obj file data stored in model
#include "model_transform.hpp"
#include "structs.hpp"

using namespace std;

using CameraPtr = shared_ptr<Camera>;
using NormalPtr = shared_ptr<Normal>;
using ReflectPtr = shared_ptr<Reflectance>;
using ModelTransformPtr = shared_ptr<ModelTransform>;

CameraPtr parse_camera_data(char *file_name) {
  ifstream obj_transform_file(file_name);
  return get_camera_data(obj_transform_file);
}

vector<Light> parse_light_data(char *file_name) {
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

vector<Light> get_light_data(ifstream& obj_transform_file) {
  vector<Light> lights = vector<Light>();
  string line;

  getline(obj_transform_file, line);
  while (line != "") {
    lights.push_back(store_light_line(line));
    getline(obj_transform_file, line);
  }

  return lights;
}

Light store_light_line(string line) {
  istringstream line_stream(line);
  string light;
  char comma1, comma2;
  double x, y, z, r, g, b, att;
  if (!(line_stream >> light >> x >> y >> z >> comma1 >> r >> g >> b >> comma2 >>att)) {
    throw "Wrong number of arguments to vertex line";
  }

  return Light(x, y, z, r, g, b, att);
}

vector<Model> store_file_objects(int argc, char **argv) {
  vector<Model> models = vector<Model>();

  for (int i = 1; i < argc; i++) {
    Model model = parse_file_to_model(string(argv[i]));
    models.push_back(model);
  }

  return models;
}

Model parse_file_to_model(string file_name) {
  ifstream obj_file(file_name);
  Model model = Model(file_name);

  string line;
  while (getline(obj_file, line)) {
    store_obj_line(line, model);
  }

  return model;
}

MaterialPtr store_material_properties(vector<string> lines) {
  MaterialPtr material = MaterialPtr(new Material());

  store_ambient_prop(lines, material);
  store_diffuse_prop(lines, material);
  store_specular_prop(lines, material);
  store_shininess_prop(lines, material);

  return material;
}

void store_ambient_prop(vector<string> lines, MaterialPtr material) {
  string line = lines[0];

  istringstream line_stream(line);
  string _;
  double r, g, b;
  if (!(line_stream >> _ >> r >> g >> b)) {
    throw "Wrong number of arguments to diffuse line";
  }

  material->ambient = ReflectPtr(new Reflectance(r, g, b));
}

void store_diffuse_prop(vector<string> lines, MaterialPtr material) {
  string line = lines[1];

  istringstream line_stream(line);
  string _;
  double r, g, b;
  if (!(line_stream >> _ >> r >> g >> b)) {
    throw "Wrong number of arguments to diffuse line";
  }

  material->diffuse = ReflectPtr(new Reflectance(r, g, b));
}

void store_specular_prop(vector<string> lines, MaterialPtr material) {
  string line = lines[2];

  istringstream line_stream(line);
  string _;
  double r, g, b;
  if (!(line_stream >> _ >> r >> g >> b)) {
    throw "Wrong number of arguments to specular line";
  }

  material->specular = ReflectPtr(new Reflectance(r, g, b));
}

void store_shininess_prop(vector<string> lines, MaterialPtr material) {
  string line = lines[3];

  istringstream line_stream(line);
  string _;
  double shiny;
  if (!(line_stream >> _ >> shiny)) {
    throw "Wrong number of arguments to shininess line";
  }

  material->shininess = shiny;
}

void store_obj_line(string line, Model &model) {
  if (is_vertex_line(line)) {
    store_vertex_line(line, model);

  } else if (is_face_line(line)) {
    store_face_line(line, model);

  } else {
    throw "Unknown line in .obj file";
  }
}

bool is_vertex_line(string line) {
  return line.at(0) == 'v' && line.at(1) == ' ';
}

bool is_face_line(string line) {
  return line.at(0) == 'f';
}

void store_vertex_line(string line, Model &model) {
  istringstream line_stream(line);
  char _;
  double x, y, z;
  if (!(line_stream >> _ >> x >> y >> z)) {
    throw "Wrong number of arguments to vertex line";
  }

  model.vertices.push_back(Vertex(x, y, z));
}

void store_face_line(string line, Model &model) {
  istringstream line_stream(line);
  char f;
  int v1, v2, v3;
  if (!(line_stream >> f >> v1 >> v2 >> v3)) {
    throw "Wrong number of arguments to face line";
  }

  model.faces.push_back(Face (v1, v2, v3));
}

shared_ptr<map<string, ModelTransformPtr>> get_objects(ifstream& obj_transform_file, CameraPtr cam) {
  shared_ptr<map<string, ModelTransformPtr>> models =
    shared_ptr<map<string, ModelTransformPtr>>(new map<string, ModelTransformPtr>());

  string line;
  getline(obj_transform_file, line); // Ignore first line
  getline(obj_transform_file, line);
  while (line != "") {
    create_obj(line, models, cam);
    getline(obj_transform_file, line);
  }

  return models;
}

void create_obj(string line, shared_ptr<map<string, ModelTransformPtr>> models, CameraPtr cam) {
  istringstream line_stream(line);

  string obj_name, obj_filename;
  if (!(line_stream >> obj_name >> obj_filename)) {
    throw "Wrong number of arguments to object line";
  }

  obj_filename = "data/" + obj_filename;

  ModelTransformPtr new_model = create_model(obj_name, obj_filename);
  new_model->cam = cam;
  (*models)[obj_name] = new_model;
}

ModelTransformPtr create_model(string obj_name, string obj_filename) {
  ModelTransformPtr transform_model =
    ModelTransformPtr(new ModelTransform());

  transform_model->model = parse_file_to_model(obj_filename.c_str());
  transform_model->copy_num = 0;
  transform_model->name = obj_name;

  return transform_model;
}
