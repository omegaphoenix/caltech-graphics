#ifndef VERTEX_H
#define VERTEX_H

// Represents a vertex of a model
struct Vertex {
  double x;
  double y;
  double z;

  Vertex(double x0, double y0, double z0) {
    x = x0;
    y = y0;
    z = z0;
  }
};

#endif
