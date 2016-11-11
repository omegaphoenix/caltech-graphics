#include "transform_obj.hpp"

#include <fstream> // basic file operations
#include <iostream>
#include <map>
#include <memory> // shared_ptr

#include "pixels.hpp"
#include "camera.hpp"
#include "geometric_transform.hpp"
#include "parser.hpp"
#include "model.hpp"
#include "model_transform.hpp"

using namespace std;

using CameraPtr = shared_ptr<Camera>;
using LightPtr = shared_ptr<Light>;
using ModelPtr = shared_ptr<Model>;
using ModelTransformPtr = shared_ptr<ModelTransform>;
using MatrixPtr = shared_ptr<Eigen::MatrixXd>;

using LightVecPtr = shared_ptr<vector<Light>>;
using ModelVectorPtr = shared_ptr<vector<ModelPtr>>;

vector<Model> parse_obj_data(char *file_name) {
  return store_obj_transform_file(file_name);
}

vector<Model> store_obj_transform_file(char *file_name) {
  ifstream obj_transform_file(file_name);
  CameraPtr cam = get_camera_data(obj_transform_file);
  // Parse light data to move ifstream along but discard
  get_light_data(obj_transform_file);
  // Parse .obj files
  shared_ptr<map<string, ModelTransformPtr>> models =
    get_objects(obj_transform_file, cam);
  // Create copies and perform geometric transformations
  vector<Model> transformed = perform_transforms(obj_transform_file, models);

  return transformed;
}

// filename lines have been removed from the ifstream
vector<Model> perform_transforms(ifstream& obj_transform_file, shared_ptr<map<string, ModelTransformPtr>> models) {
  vector<Model> trans_models = vector<Model>();

  vector<string> lines;

  string line;
  while (getline(obj_transform_file, line)) {
    if (line == "") {
      // Perform geo transforms to world coord. and store material properties
      trans_models.push_back(perform_transform(lines, models));
      lines.erase(lines.begin(), lines.end());
    } else {
      lines.push_back(line);
    }
  }

  if (!lines.empty()) {
    trans_models.push_back(perform_transform(lines, models));
  }

  return trans_models;
}

Model perform_transform(vector<string> lines, shared_ptr<map<string, ModelTransformPtr>> models) {
  // Remove name from vector of lines
  string name = lines.front();
  lines.erase(lines.begin());

  MaterialPtr material = store_material_properties(lines);
  lines.erase(lines.begin(), lines.begin() + 4);

  MatrixPtr trans_mat = multiply_matrices(lines);
  MatrixPtr norm_trans_mat = create_norm_trans_mat(lines);

  // Apply geometric transforms to vertices and normals
  Model new_copy = (*models)[name]->apply_trans_mat(trans_mat, norm_trans_mat);
  new_copy.material = material;
  new_copy.set_variables();
  return new_copy;
}

Pixel **new_grid(int xres, int yres) {
  Pixel **grid = new Pixel *[yres];
  for (int y = 0; y < yres; y++) {
    grid[y] = new Pixel[xres];
    for (int x = 0; x < xres; x++) {
      grid[y][x].red = 0;
      grid[y][x].blue = 0;
      grid[y][x].green = 0;
    }
  }
  return grid;
}

void delete_grid(int xres, int yres, Pixel **grid) {
  for (int y = 0; y < yres; y++) {
    delete[] grid[y];
  }
}
