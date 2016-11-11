#ifndef TRANSFORM_OBJ_HPP
#define TRANSFORM_OBJ_HPP

#include <fstream> // basic file operations
#include <map>
#include <memory> // shared_ptr
#include <string>
#include <vector>

#include "camera.hpp"
#include "pixels.hpp"
#include "model.hpp"
#include "model_transform.hpp"

using namespace std;

using ModelTransformPtr = shared_ptr<ModelTransform>;

// store original objects from .obj files
vector<Model> store_obj_transform_file(char *file_name);

// transforms helper function for all transforms
vector<Model> perform_transforms(ifstream& obj_transform_file, shared_ptr<map<string, ModelTransformPtr>> models);
// transforms helper function for one copy
Model perform_transform(vector<string> lines, shared_ptr<map<string, ModelTransformPtr>> models);

// Create new 2D array of pixels
Pixel **new_grid(int xres, int yres);
// Delete 2D array of pixels
void delete_grid(int xres, int yres, Pixel **grid);

#endif
