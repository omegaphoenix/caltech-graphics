#ifndef FACE_HPP
#define FACE_HPP

// Represents a triangular face of a model
struct Face {
  int vertex1, vertex2, vertex3;

  int normal1, normal2, normal3;

  Face (int v1, int v2, int v3, int n1, int n2, int n3) {
    vertex1 = v1;
    vertex2 = v2;
    vertex3 = v3;

    normal1 = n1;
    normal2 = n2;
    normal3 = n3;
  }
};

#endif
