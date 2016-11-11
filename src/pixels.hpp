#ifndef PIXELS_HPP
#define PIXELS_HPP

using namespace std;

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

#endif
