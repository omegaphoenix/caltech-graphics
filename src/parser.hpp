#ifndef OBJ_FILE_PARSER_HPP
#define OBJ_FILE_PARSER_HPP

#include <fstream> // basic file operations
#include <map>
#include <memory> // shared_ptr
#include <string>
#include <vector>

#include "camera.hpp"
#include "three_d_model.hpp" // .obj file data stored in model
#include "three_d_model_transform.hpp"

using namespace std;

using CameraPtr = shared_ptr<Camera>;
using ThreeDModelPtr = shared_ptr<ThreeDModel>;
using ThreeDModelTransformPtr = shared_ptr<ThreeDModelTransform>;

// parse camera lines
CameraPtr get_camera_data(ifstream& obj_transform_file);

// parses multiple .obj files
shared_ptr<vector<ThreeDModelPtr>> store_file_objects(int argc, char **argv);

// helper function - parse one file
ThreeDModelPtr parse_file_to_model(string file_name);

// store one line of file as face or vertex
void store_line(string line, ThreeDModelPtr model);

// return true if vertex line
bool is_vertex_line(string line);
// return true if face line
bool is_face_line(string line);
// helper functions for storing lines of file
void store_vertex_line(string line, ThreeDModelPtr model);
void store_face_line(string line, ThreeDModelPtr model);

// parsing helper functions
// helper function to get objects from .obj files
shared_ptr<map<string, ThreeDModelTransformPtr>> get_objects(ifstream& obj_transform_file, CameraPtr cam);
// helper function to create objects from line and place it in the map
void create_obj(string line, shared_ptr<map<string, ThreeDModelTransformPtr>> models, CameraPtr cam);
// helper function to create new object
ThreeDModelTransformPtr create_model(string obj_name, string obj_filename);
// convert from string to char *
char *convert_to_char_arr(string input_string);

#endif
