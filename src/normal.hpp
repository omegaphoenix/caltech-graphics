#ifndef NORMAL_HPP
#define NORMAL_HPP

#include <math.h> // sqrt

// Represents a// Represents a normal of a surface
struct Normal {
  float x;
  float y;
  float z;

  Normal() {
  }

  Normal(float x0, float y0, float z0) {
    set_normal(x0, y0, z0);
  }

  // Normalize normal on creation
  void set_normal(float x0, float y0, float z0) {
    float magnitude = sqrt(x0*x0 + y0*y0 + z0*z0);
    x = x0/magnitude;
    y = y0/magnitude;
    z = z0/magnitude;
  }
};

#endif
