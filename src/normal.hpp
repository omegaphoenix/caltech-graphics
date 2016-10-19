#ifndef NORMAL_H
#define NORMAL_H

// Represents a// Represents a normal of a surface
struct Normal {
  double x;
  double y;
  double z;

  Normal(double x0, double y0, double z0) {
    set_normal(x0, y0, z0);
  }

  void set_normal(double x0, double y0, double z0) {
    x = x0;
    y = y0;
    z = z0;
  }
};

#endif
