#ifndef VERTEX_HPP
#define VERTEX_HPP

// Represents a vertex of a model
struct Vertex {
  float x;
  float y;
  float z;

  Vertex() {
    set_vertex(0, 0, 0);
  }

  Vertex(double x0, double y0, double z0) {
    set_vertex(x0, y0, z0);
  }

  void set_vertex(double x0, double y0, double z0) {
    x = x0;
    y = y0;
    z = z0;
  }
};

#endif
