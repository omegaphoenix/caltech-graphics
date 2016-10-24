#ifndef LIGHT_HPP
#define LIGHT_HPP

// Represents a point light
struct Light {
  double x, y, z;
  double r, g, b;
  double attenuation;

  Light (double _x, double _y, double _z, double _r, double _g, double _b, double att) {
    x = _x;
    y = _y;
    z = _z;
    r = _r;
    g = _g;
    b = _b;
    attenuation = att;
  }
};

#endif
