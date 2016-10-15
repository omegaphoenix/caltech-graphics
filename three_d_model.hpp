#ifndef THREE_D_MODEL_HPP
#define THREE_D_MODEL_HPP

#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "draw_pixels.hpp"
#include "face.hpp"
#include "vertex.hpp"

using namespace std;

struct ThreeDModel {
  string name;
  vector<shared_ptr<Vertex> > *vertices;
  vector<shared_ptr<Face> > *faces;

  // empty constructor
  ThreeDModel() {
  }

  // constructor using file
  ThreeDModel(char *raw_file_name) {
    name = get_name(raw_file_name);
    setup_vertices();
    faces = new vector<shared_ptr<Face> >();
  }

  // helper function for constructor to get object name
  string get_name(char *raw_file_name) {
    // Remove .obj from name
    string str_name(raw_file_name);
    return str_name.substr(0, str_name.size() - 4);
  }

  // helper function for constructor to get vertices
  void setup_vertices() {
    vertices = new vector<shared_ptr<Vertex> >();
    // Index 0 is NULL because vertices are 1-indexed
    vertices->push_back(NULL);
  }

  // Draw model on the grid representing the screen
  void draw_model(int xres, int yres, Pixel **grid) {
    for (vector<shared_ptr<Face> >::iterator face_it = faces->begin(); face_it != faces->end(); face_it++) {
      draw_face(xres, yres, *face_it, grid);
    }
  }

  // Draw a single face on the grid representing the screen
  void draw_face(int xres, int yres, shared_ptr<Face> face, Pixel **grid) {
    shared_ptr<Vertex> v1 = NDC_to_pixel(xres, yres, (*vertices)[face->vertex1]);
    shared_ptr<Vertex> v2 = NDC_to_pixel(xres, yres, (*vertices)[face->vertex2]);
    shared_ptr<Vertex> v3 = NDC_to_pixel(xres, yres, (*vertices)[face->vertex3]);

    rasterize(v1, v2, grid, xres, yres);
    rasterize(v2, v3, grid, xres, yres);
    rasterize(v3, v1, grid, xres, yres);
  }
};

#endif
