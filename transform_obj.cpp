#include "transform_obj.hpp"

#include <fstream> // basic file operations
#include <iostream>
#include <map>
#include <memory> // shared_ptr

#include "draw_pixels.hpp"
#include "camera.hpp"
#include "geometric_transform.hpp"
#include "parser.hpp"
#include "three_d_model.hpp"
#include "three_d_model_transform.hpp"

using namespace std;

shared_ptr<vector<shared_ptr<ThreeDModel>>> store_obj_transform_file(char *file_name) {
  ifstream obj_transform_file(file_name);
  shared_ptr<Camera> cam = get_camera_data(obj_transform_file);
  shared_ptr<map<string, shared_ptr<ThreeDModelTransform>>> models =
    get_objects(obj_transform_file, cam);
  shared_ptr<vector<shared_ptr<ThreeDModel>>> transformed =
    perform_transforms(obj_transform_file, models);

  return transformed;
}

// filename lines have been removed from the ifstream
shared_ptr<vector<shared_ptr<ThreeDModel>>> perform_transforms(ifstream& obj_transform_file, shared_ptr<map<string, shared_ptr<ThreeDModelTransform>>> models) {
  shared_ptr<vector<shared_ptr<ThreeDModel>>> trans_models = shared_ptr<vector<shared_ptr<ThreeDModel>>>(new vector<shared_ptr<ThreeDModel>>());

  vector<string> lines;

  string line;
  while (getline(obj_transform_file, line)) {
    if (line == "") {
      trans_models->push_back(perform_transform(lines, models));
      lines.erase(lines.begin(), lines.end());
    } else {
      lines.push_back(line);
    }
  }

  if (!lines.empty()) {
    trans_models->push_back(perform_transform(lines, models));
  }

  return trans_models;
}

shared_ptr<ThreeDModel> perform_transform(vector<string> lines, shared_ptr<map<string, shared_ptr<ThreeDModelTransform>>> models) {
  // Remove name from vector of lines
  string name = lines.front();
  lines.erase(lines.begin());

  shared_ptr<Eigen::MatrixXd> trans_mat = multiply_matrices(lines);
  return (*models)[name]->apply_trans_mat(trans_mat);
}

void print_ppm(int xres, int yres, shared_ptr<vector<shared_ptr<ThreeDModel>>> models) {
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

void print_transformed_vertices(shared_ptr<vector<shared_ptr<ThreeDModel>>> models) {
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
  shared_ptr<vector<shared_ptr<Vertex>>> vertices = model->vertices;

  // 0-indexed vertex is NULL
  vector<shared_ptr<Vertex> >::iterator vertex_it = ++(vertices->begin());
  while (vertex_it != vertices->end()) {
    cout << (*vertex_it)->x << " " << (*vertex_it)->y << " " << (*vertex_it)->z << endl;
    ++vertex_it;
  }
}
