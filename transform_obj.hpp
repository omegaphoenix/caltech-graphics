#ifndef TRANSFORM_OBJ_HPP
#define TRANSFORM_OBJ_HPP

#include <fstream> // basic file operations
#include <map>
#include <string>
#include <vector>

#include "three_d_model_transform.hpp"
#include "three_d_model.hpp"

using namespace std;

// main method - store original objects
vector<ThreeDModel *> *store_obj_transform_file(char *file_name);

// parsing helper functions
// helper function to get objects from .obj files
map<string, ThreeDModelTransform *> *get_objects(ifstream& obj_transform_file);
// helper function to create objects from line and place it in the map
void create_obj(string line, map<string, ThreeDModelTransform *> *models);
// helper function to create new object
ThreeDModelTransform *create_model(string obj_name, string obj_filename);
// convert from string to char *
char *convert_to_char_arr(string input_string);

// transforms helper function for all transforms
vector<ThreeDModel *> *perform_transforms(ifstream& obj_transform_file, map<string, ThreeDModelTransform *> *models);
// transforms helper function for one copy
ThreeDModel *perform_transform(vector<string> *lines, map<string, ThreeDModelTransform *> *models);

// main method - create copies, transform vectors, and print name and vectors
void print_transformed_vertices(vector<ThreeDModel *> *models);

// helper function to print model
void print(ThreeDModel *model);
// helper function to print model vertices
void print_vertices(ThreeDModel *model);

#endif
