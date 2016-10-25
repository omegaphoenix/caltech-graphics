#ifndef THREE_D_MODEL_HPP
#define THREE_D_MODEL_HPP

#include <cstddef>
#include <iostream>
#include <memory> // shared_ptr
#include <string>
#include <vector>

#include "draw_pixels.hpp"
#include "face.hpp"
#include "normal.hpp"
#include "vertex.hpp"

using namespace std;

struct Pixel;

using FacePtr = shared_ptr<Face>;
using MaterialPtr = shared_ptr<struct Material>;
using NormalPtr = shared_ptr<Normal>;
using ReflectPtr = shared_ptr<struct Reflectance>;
using VertexPtr = shared_ptr<Vertex>;

using VerVectorPtr = shared_ptr<vector<VertexPtr>>;
using NormVectorPtr = shared_ptr<vector<NormalPtr>>;


// Represents the reflectance property of a material
struct Reflectance {
  double red, green, blue;

  Reflectance(double r, double g, double b) {
    red = r;
    green = g;
    blue = b;
  }
};

// Represents the material properties of an object
struct Material {
  ReflectPtr ambient, diffuse, specular;
  double shininess;

  Material() {
  }
};

class ThreeDModel {
  public:
    string name;
    VerVectorPtr vertices;
    NormVectorPtr normals;
    shared_ptr<vector<FacePtr>> faces;
    MaterialPtr material;

    vector<Vertex> vertex_buffer;
    vector<Normal> normal_buffer;

    /* Index 0 has the r-component
     * Index 1 has the g-component
     * Index 2 has the b-component
     */
    float ambient_reflect[3];
    float diffuse_reflect[3];
    float specular_reflect[3];

    float shininess;

    // empty constructor
    ThreeDModel();
    // constructor using file
    ThreeDModel(string raw_file_name);

    // helper function for constructor to get object name
    string get_name(string raw_file_name);
    // helper function for constructor to setup vertices
    void setup_vertices();
    // helper function for constructor to setup normals
    void setup_normals();
    // Draw model on the grid representing the screen
    void draw_model(int xres, int yres, Pixel **grid);
    // Draw a single face on the grid representing the screen
    void draw_face(int xres, int yres, FacePtr face, Pixel **grid);
};

#endif
