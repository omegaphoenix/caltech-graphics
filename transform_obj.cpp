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

using CameraPtr = shared_ptr<Camera>;
using ThreeDModelPtr = shared_ptr<ThreeDModel>;
using ThreeDModelTransformPtr = shared_ptr<ThreeDModelTransform>;
using MatrixPtr = shared_ptr<Eigen::MatrixXd>;
using VertexPtr = shared_ptr<Vertex>;
using ModelVectorPtr = shared_ptr<vector<ThreeDModelPtr>>;
using VerVectorPtr = shared_ptr<vector<VertexPtr>>;

ModelVectorPtr store_obj_transform_file(char *file_name) {
  ifstream obj_transform_file(file_name);
  CameraPtr cam = get_camera_data(obj_transform_file);
  shared_ptr<map<string, ThreeDModelTransformPtr>> models =
    get_objects(obj_transform_file, cam);
  ModelVectorPtr transformed =
    perform_transforms(obj_transform_file, models);

  return transformed;
}

// filename lines have been removed from the ifstream
ModelVectorPtr perform_transforms(ifstream& obj_transform_file, shared_ptr<map<string, ThreeDModelTransformPtr>> models) {
  ModelVectorPtr trans_models = ModelVectorPtr(new vector<ThreeDModelPtr>());

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

ThreeDModelPtr perform_transform(vector<string> lines, shared_ptr<map<string, ThreeDModelTransformPtr>> models) {
  // Remove name from vector of lines
  string name = lines.front();
  lines.erase(lines.begin());

  MatrixPtr trans_mat = multiply_matrices(lines);
  return (*models)[name]->apply_trans_mat(trans_mat);
}

void print_ppm(int xres, int yres, ModelVectorPtr models) {
  Pixel **grid = new_grid(xres, yres);

  for (vector<ThreeDModelPtr>::iterator model_it = models->begin(); model_it != models->end(); ++model_it) {
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

void print_transformed_vertices(ModelVectorPtr models) {
  for (vector<ThreeDModelPtr>::iterator model_it = models->begin(); model_it != models->end(); ++model_it) {
    print(*model_it);
  }
}

void print(ThreeDModelPtr model) {
  cout << model->name << endl;
  print_vertices(model);
  cout << endl;
}

void print_vertices(ThreeDModelPtr model) {
  VerVectorPtr vertices = model->vertices;

  // 0-indexed vertex is NULL
  vector<VertexPtr>::iterator vertex_it = ++(vertices->begin());
  while (vertex_it != vertices->end()) {
    cout << (*vertex_it)->x << " " << (*vertex_it)->y << " " << (*vertex_it)->z << endl;
    ++vertex_it;
  }
}
