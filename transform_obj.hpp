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

// main method - store original objects
shared_ptr<vector<shared_ptr<ThreeDModel>>> store_obj_transform_file(char *file_name);

// transforms helper function for all transforms
shared_ptr<vector<shared_ptr<ThreeDModel>>> perform_transforms(ifstream& obj_transform_file, shared_ptr<map<string, shared_ptr<ThreeDModelTransform>>> models);
// transforms helper function for one copy
shared_ptr<ThreeDModel> perform_transform(vector<string> lines, shared_ptr<map<string, shared_ptr<ThreeDModelTransform>>> models);

// main method - create copies, transform vectors, and print name and vectors
void print_ppm(int xres, int yres, shared_ptr<vector<shared_ptr<ThreeDModel>>> models);
Pixel **new_grid(int xres, int yres);
void delete_grid(int xres, int yres, Pixel **grid);
void print_transformed_vertices(shared_ptr<vector<shared_ptr<ThreeDModel>>> models);

// helper function to print model
void print(shared_ptr<ThreeDModel> model);
// helper function to print model vertices
void print_vertices(shared_ptr<ThreeDModel> model);

#endif
