#ifndef TRANSFORM_OBJ_HPP
#define TRANSFORM_OBJ_HPP

#include <fstream> // basic file operations
#include <map>
#include <string>
#include <vector>

#include "draw_pixels.hpp"
#include "camera.hpp"
#include "three_d_model_transform.hpp"
#include "three_d_model.hpp"

using namespace std;

// main method - store original objects
vector<shared_ptr<ThreeDModel> > *store_obj_transform_file(char *file_name);

// parsing helper functions
// parse camera lines
shared_ptr<Camera> get_camera_data(ifstream& obj_transform_file);
// helper function to get objects from .obj files
map<string, shared_ptr<ThreeDModelTransform> > *get_objects(ifstream& obj_transform_file, shared_ptr<Camera> cam);
// helper function to create objects from line and place it in the map
void create_obj(string line, map<string, shared_ptr<ThreeDModelTransform> > *models, shared_ptr<Camera> cam);
// helper function to create new object
shared_ptr<ThreeDModelTransform> create_model(string obj_name, string obj_filename);
// convert from string to char *
char *convert_to_char_arr(string input_string);

// transforms helper function for all transforms
vector<shared_ptr<ThreeDModel> > *perform_transforms(ifstream& obj_transform_file, map<string, shared_ptr<ThreeDModelTransform> > *models);
// transforms helper function for one copy
shared_ptr<ThreeDModel> perform_transform(vector<string> lines, map<string, shared_ptr<ThreeDModelTransform> > *models);

// main method - create copies, transform vectors, and print name and vectors
void print_ppm(int xres, int yres, vector<shared_ptr<ThreeDModel> > *models);
Pixel **new_grid(int xres, int yres);
void delete_grid(int xres, int yres, Pixel **grid);
void print_transformed_vertices(vector<shared_ptr<ThreeDModel> > *models);

// helper function to print model
void print(shared_ptr<ThreeDModel> model);
// helper function to print model vertices
void print_vertices(shared_ptr<ThreeDModel> model);

#endif
