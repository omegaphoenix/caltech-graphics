#ifndef THREE_D_MODEL_HPP
#define THREE_D_MODEL_HPP

#include <cstddef>
#include <iostream>
#include <memory> // shared_ptr
#include <string>
#include <vector>

#include "draw_pixels.hpp"
#include "face.hpp"
#include "vertex.hpp"

using namespace std;

using FacePtr = shared_ptr<Face>;
using VertexPtr = shared_ptr<Vertex>;
using VerVectorPtr = shared_ptr<vector<VertexPtr>>;

class ThreeDModel {
  public:
    string name;
    VerVectorPtr vertices;
    shared_ptr<vector<FacePtr>> faces;

    // empty constructor
    ThreeDModel();
    // constructor using file
    ThreeDModel(string raw_file_name);

    // helper function for constructor to get object name
    string get_name(string raw_file_name);
    // helper function for constructor to get vertices
    void setup_vertices();
    // Draw model on the grid representing the screen
    void draw_model(int xres, int yres, Pixel **grid);
    // Draw a single face on the grid representing the screen
    void draw_face(int xres, int yres, FacePtr face, Pixel **grid);
};

#endif
