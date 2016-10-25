#ifndef TRANSFORM_OBJ_HPP
#define TRANSFORM_OBJ_HPP

#include <fstream> // basic file operations
#include <map>
#include <memory> // shared_ptr
#include <string>
#include <vector>

#include "camera.hpp"
#include "draw_pixels.hpp"
#include "three_d_model.hpp"
#include "three_d_model_transform.hpp"

using namespace std;

using ModelPtr = shared_ptr<Model>;
using ModelTransformPtr = shared_ptr<ModelTransform>;
using ModelVectorPtr = shared_ptr<vector<ModelPtr>>;

// store original objects from .obj files
ModelVectorPtr store_obj_transform_file(char *file_name);

// transforms helper function for all transforms
ModelVectorPtr perform_transforms(ifstream& obj_transform_file, shared_ptr<map<string, ModelTransformPtr>> models);
// transforms helper function for one copy
ModelPtr perform_transform(vector<string> lines, shared_ptr<map<string, ModelTransformPtr>> models);

// create copies, transform vectors, and print name and vectors
void print_ppm(int xres, int yres, ModelVectorPtr models);

// Create new 2D array of pixels
Pixel **new_grid(int xres, int yres);
// Delete 2D array of pixels
void delete_grid(int xres, int yres, Pixel **grid);

// Print all vertices and normals
void print_transformed_vertices(ModelVectorPtr models);
void print_transformed_normals(ModelVectorPtr models);

// helper functions to print model name and vertices and normals
void print_model_vertices(ModelPtr model);
void print_model_normals(ModelPtr model);
// Prints vertices and normals
void print_vertices(ModelPtr model);
void print_normals(ModelPtr model);

#endif
