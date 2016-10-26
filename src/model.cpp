#include "model.hpp"

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

using FacePtr = shared_ptr<Face>;
using MaterialPtr = shared_ptr<Material>;

using FaceVectorPtr = shared_ptr<vector<FacePtr>>;

// empty constructor
Model :: Model() {
  name = "";
  setup_vertices();
  setup_normals();
  faces = FaceVectorPtr(new vector<FacePtr>());
  material = MaterialPtr(new Material());
}

// constructor using file
Model :: Model(string raw_file_name) {
  name = get_name(raw_file_name);
  setup_vertices();
  setup_normals();
  faces = FaceVectorPtr(new vector<FacePtr>());
  material = MaterialPtr(new Material());
}

// helper function for constructor to get object name
string Model :: get_name(string raw_file_name) {
  // Remove .obj from name
  return raw_file_name.substr(0, raw_file_name.size() - 4);
}

// helper function for constructor to get vertices
void Model :: setup_vertices() {
  vertex_buffer = vector<Vertex>();
  // Index 0 is filler because vertices are 1-indexed
  vertex_buffer.push_back(Vertex());
}

// helper function for constructor to get normals
void Model :: setup_normals() {
  normal_buffer = vector<Normal>();
  // Index 0 is filler because vertices are 1-indexed
  normal_buffer.push_back(Normal());
}

// Draw model on the grid representing the screen
void Model :: draw_model(int xres, int yres, Pixel **grid) {
  for (vector<FacePtr>::iterator face_it = faces->begin(); face_it != faces->end(); face_it++) {
    draw_face(xres, yres, *face_it, grid);
  }
}

// Draw a single face on the grid representing the screen
void Model :: draw_face(int xres, int yres, FacePtr face, Pixel **grid) {
  Vertex v1 = NDC_to_pixel(xres, yres, vertex_buffer[face->vertex1]);
  Vertex v2 = NDC_to_pixel(xres, yres, vertex_buffer[face->vertex2]);
  Vertex v3 = NDC_to_pixel(xres, yres, vertex_buffer[face->vertex3]);

  rasterize(v1, v2, grid, xres, yres);
  rasterize(v2, v3, grid, xres, yres);
  rasterize(v3, v1, grid, xres, yres);
}
