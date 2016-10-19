#include "three_d_model.hpp"

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

// empty constructor
ThreeDModel :: ThreeDModel() {
}

// constructor using file
ThreeDModel :: ThreeDModel(string raw_file_name) {
  name = get_name(raw_file_name);
  setup_vertices();
  faces = shared_ptr<vector<FacePtr>>(new vector<FacePtr>());
}

// helper function for constructor to get object name
string ThreeDModel :: get_name(string raw_file_name) {
  // Remove .obj from name
  return raw_file_name.substr(0, raw_file_name.size() - 4);
}

// helper function for constructor to get vertices
void ThreeDModel :: setup_vertices() {
  vertices = VerVectorPtr(new vector<VertexPtr>());
  // Index 0 is NULL because vertices are 1-indexed
  vertices->push_back(NULL);
}

// Draw model on the grid representing the screen
void ThreeDModel :: draw_model(int xres, int yres, Pixel **grid) {
  for (vector<FacePtr>::iterator face_it = faces->begin(); face_it != faces->end(); face_it++) {
    draw_face(xres, yres, *face_it, grid);
  }
}

// Draw a single face on the grid representing the screen
void ThreeDModel :: draw_face(int xres, int yres, FacePtr face, Pixel **grid) {
  VertexPtr v1 = NDC_to_pixel(xres, yres, (*vertices)[face->vertex1]);
  VertexPtr v2 = NDC_to_pixel(xres, yres, (*vertices)[face->vertex2]);
  VertexPtr v3 = NDC_to_pixel(xres, yres, (*vertices)[face->vertex3]);

  rasterize(v1, v2, grid, xres, yres);
  rasterize(v2, v3, grid, xres, yres);
  rasterize(v3, v1, grid, xres, yres);
}
