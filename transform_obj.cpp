#include "transform_obj.hpp"

#include <cstring>
#include <fstream> // basic file operations
#include <iostream>
#include <map>

#include "draw_pixels.hpp"
#include "camera.hpp"
#include "geometric_transform.hpp"
#include "obj_file_parser.hpp"
#include "three_d_model_transform.hpp"

using namespace std;

Camera *cam;

vector<shared_ptr<ThreeDModel> > *store_obj_transform_file(char *file_name) {
  ifstream obj_transform_file(file_name);
  cam = get_camera_data(obj_transform_file);
  map<string, ThreeDModelTransform *> *models = get_objects(obj_transform_file, cam);
  vector<shared_ptr<ThreeDModel> > *transformed = perform_transforms(obj_transform_file, models);
  return transformed;
}

Camera *get_camera_data(ifstream& obj_transform_file) {
  vector<string> *lines = new vector<string>;
  string line;

  getline(obj_transform_file, line);
  while (line != "") {
    lines->push_back(line);
    getline(obj_transform_file, line);
  }

  Camera *cam = new Camera(lines);
  return cam;
}

map<string, ThreeDModelTransform *> *get_objects(ifstream& obj_transform_file, Camera *cam) {
  map<string, ThreeDModelTransform *> *models = new map<string, ThreeDModelTransform *>();

  string line;
  getline(obj_transform_file, line);
  getline(obj_transform_file, line);
  while (line != "") {
    create_obj(line, models, cam);
    getline(obj_transform_file, line);
  }

  return models;
}

void create_obj(string line, map<string, ThreeDModelTransform *> *models, Camera *cam) {
  istringstream line_stream(line);

  string obj_name, obj_filename;
  if (!(line_stream >> obj_name >> obj_filename)) {
    throw "Wrong number of arguments to object line";
  }

  obj_filename = "data/" + obj_filename;

  ThreeDModelTransform *new_model = create_model(obj_name, obj_filename);
  new_model->cam = cam;
  (*models)[obj_name] = new_model;
}

ThreeDModelTransform *create_model(string obj_name, string obj_filename) {
  ThreeDModelTransform *transform_model = new ThreeDModelTransform();

  char *filename = convert_to_char_arr(obj_filename);

  transform_model->model = parse_file_to_model(filename);
  transform_model->copy_num = 0;
  transform_model->name = obj_name;
  return transform_model;
}

char *convert_to_char_arr(string input_string) {
  // allocate memory dynamically and copy content of string to char *
  char *char_arr = strdup(input_string.c_str());
  return char_arr;
}

// filename lines have been removed from the ifstream
vector<shared_ptr<ThreeDModel> > *perform_transforms(ifstream& obj_transform_file, map<string, ThreeDModelTransform *> *models) {
  vector<shared_ptr<ThreeDModel> > *trans_models = new vector<shared_ptr<ThreeDModel> >();

  vector<string> *lines = new vector<string>;

  string line;
  while (getline(obj_transform_file, line)) {
    if (line == "") {
      trans_models->push_back(perform_transform(lines, models));
      lines = new vector<string>;
    } else {
      lines->push_back(line);
    }
  }

  if (!lines->empty()) {
    trans_models->push_back(perform_transform(lines, models));
  }

  return trans_models;
}

shared_ptr<ThreeDModel> perform_transform(vector<string> *lines, map<string, ThreeDModelTransform *> *models) {
  // Remove name from vector of lines
  string name = lines->front();
  lines->erase(lines->begin());

  shared_ptr<Eigen::MatrixXd> trans_mat = multiply_matrices(lines);
  return (*models)[name]->apply_trans_mat(trans_mat);
}

void print_ppm(int xres, int yres, vector<shared_ptr<ThreeDModel> > *models) {
  Pixel **grid = new_grid(xres, yres);

  for (vector<shared_ptr<ThreeDModel> >::iterator model_it = models->begin(); model_it != models->end(); ++model_it) {
    (*model_it)->draw_model(xres, yres, grid);
  }

  output_ppm(xres, yres, grid);
  delete_grid(xres, yres, grid);
  delete[] grid;
}

Pixel **new_grid(int xres, int yres) {
  Pixel **grid = new Pixel *[yres];
  for (int y = 0; y < yres; y++) {
    for (int x = 0; x < xres; x++) {
      if (x == 0) {
        grid[y] = new Pixel[xres];
      }
      grid[y][x].colored = false;
    }
  }
  return grid;
}

void delete_grid(int xres, int yres, Pixel **grid) {
  for (int y = 0; y < yres; y++) {
    delete[] grid[y];
  }
}

void print_transformed_vertices(vector<shared_ptr<ThreeDModel> > *models) {
  for (vector<shared_ptr<ThreeDModel> >::iterator model_it = models->begin(); model_it != models->end(); ++model_it) {
    print(*model_it);
  }
}

void print(shared_ptr<ThreeDModel> model) {
  cout << model->name << endl;
  print_vertices(model);
  cout << endl;
}

void print_vertices(shared_ptr<ThreeDModel> model) {
  vector<shared_ptr<Vertex> > *vertices = model->vertices;

  // 0-indexed vertex is NULL
  vector<shared_ptr<Vertex> >::iterator vertex_it = ++(vertices->begin());
  while (vertex_it != vertices->end()) {
    cout << (*vertex_it)->x << " " << (*vertex_it)->y << " " << (*vertex_it)->z << endl;
    ++vertex_it;
  }
}
