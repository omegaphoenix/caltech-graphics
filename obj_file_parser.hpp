#ifndef OBJ_FILE_PARSER_HPP
#define OBJ_FILE_PARSER_HPP

#include <string>
#include <vector>

#include "three_d_model.hpp" // .obj file data stored in model

using namespace std;

// main method - parses multiple files
vector<ThreeDModel *> *store_file_objects(int argc, char **argv);

// helper function - parse one file
ThreeDModel *parse_file_to_model(char *file_name);

// store one line of file as face or vertex
void store_line(string line, ThreeDModel *model);

// return true if vertex line
bool is_vertex_line(string line);
// return true if face line
bool is_face_line(string line);
// helper functions for storing lines of file
void store_vertex_line(string line, ThreeDModel *model);
void store_face_line(string line, ThreeDModel *model);

#endif
