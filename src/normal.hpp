#ifndef NORMAL_H
#define NORMAL_H

#include <math.h> // sqrt

// Represents a// Represents a normal of a surface
struct Normal {
  double x;
  double y;
  double z;

  Normal(double x0, double y0, double z0) {
    set_normal(x0, y0, z0);
  }

  void set_normal(double x0, double y0, double z0) {
    double magnitude = sqrt(x0*x0 + y0*y0 + z0*z0);
    x = x0/magnitude;
    y = y0/magnitude;
    z = z0/magnitude;
  }
};

#endif
