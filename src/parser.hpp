#ifndef PARSER_HPP
#define PARSER_HPP

#include <fstream> // basic file operations
#include <map>
#include <memory> // shared_ptr
#include <string>
#include <vector>

#include "camera.hpp"
#include "light.hpp"
#include "model.hpp" // .obj file data stored in model
#include "model_transform.hpp"

using namespace std;

using CameraPtr = shared_ptr<Camera>;
using LightPtr = shared_ptr<Light>;
using ModelPtr = shared_ptr<Model>;
using ModelTransformPtr = shared_ptr<ModelTransform>;

using LightVecPtr = shared_ptr<vector<Light>>;

// parse camera and light lines
CameraPtr parse_camera_data(char *file_name);
vector<Light> parse_light_data(char *file_name);

// helper functions for parsing camera and light lines
CameraPtr get_camera_data(ifstream& obj_transform_file);
vector<Light> get_light_data(ifstream& obj_transform_file);

// adds Light to lights
Light store_light_line(string line);

// parses multiple .obj files
vector<Model> store_file_objects(int argc, char **argv);

// store material properties of object copy
MaterialPtr store_material_properties(vector<string> lines);
void store_ambient_prop(vector<string> lines, MaterialPtr material);
void store_diffuse_prop(vector<string> lines, MaterialPtr material);
void store_specular_prop(vector<string> lines, MaterialPtr material);
void store_shininess_prop(vector<string> lines, MaterialPtr material);

// helper function for parsing one file
Model parse_file_to_model(string file_name);

// store one line of file as face or vertex
void store_obj_line(string line, Model model);

// helper functions for identifying lines of file
bool is_vertex_line(string line);
bool is_normal_line(string line);
bool is_face_line(string line);

// helper functions for storing lines of file
void store_vertex_line(string line, Model model);
void store_normal_line(string line, Model model);
void store_face_line(string line, Model model);

// helper function to get objects from .obj files
shared_ptr<map<string, ModelTransformPtr>> get_objects(ifstream& obj_transform_file, CameraPtr cam);
// helper function to create objects from line and place it in the map
void create_obj(string line, shared_ptr<map<string, ModelTransformPtr>> models, CameraPtr cam);
// helper function to create new object
ModelTransformPtr create_model(string obj_name, string obj_filename);
// convert from string to char *
char *convert_to_char_arr(string input_string);

#endif
