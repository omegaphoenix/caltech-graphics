#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <math.h>
#include <vector>

using namespace std;

struct Vec3f
{
	float x, y, z;
};

// Represents the normal of a vertex
struct Normal {
  float x;
  float y;
  float z;

  Normal(float x0, float y0, float z0) {
    set_normal(x0, y0, z0);
  }

  // Normalize normal on creation
  void set_normal(float x0, float y0, float z0) {
    float magnitude = sqrt(x0*x0 + y0*y0 + z0*z0);
    if (magnitude == 0) {
      throw "Normal vector can't be 0";
    }

    x = x0/magnitude;
    y = y0/magnitude;
    z = z0/magnitude;
  }
};

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

struct Face {
  int vertex1, vertex2, vertex3;

  Face (int v1, int v2, int v3) {
    vertex1 = v1;
    vertex2 = v2;
    vertex3 = v3;
  }
};

// Represents the rgb values for one pixel on the screen
struct Pixel {
  // [0, 1]
  double red, green, blue;

  Pixel() {
    red = 1;
    green = 1;
    blue = 1;
  }

  Pixel(double r, double g, double b) {
    red = r;
    green = g;
    blue = b;
  }
};

// Represents a point light
struct Light {
  float position[4];
  float color[3];
  float attenuation_k;

  Light() {
  }

  Light(float x, float y, float z, float r, float g, float b, float att) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
    position[3] = 1;
    color[0] = r;
    color[1] = g;
    color[2] = b;
    attenuation_k = att;
  }
};

struct Mesh_Data
{
    vector<Vertex> *vertices;
    vector<Face> *faces;
};

#endif
