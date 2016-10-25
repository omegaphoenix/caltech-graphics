#ifndef LIGHT_HPP
#define LIGHT_HPP

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

#endif
