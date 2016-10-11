#ifndef FACE_HPP
#define FACE_HPP

// Represents a triangular face of a model
struct Face {
  int vertex1;
  int vertex2;
  int vertex3;

  Face(int v1, int v2, int v3) {
    vertex1 = v1;
    vertex2 = v2;
    vertex3 = v3;
  }
};

#endif
