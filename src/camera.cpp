#include "camera.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "geometric_transform.hpp"
#include "structs.hpp"

#include "Eigen/Dense"

using namespace std;

// Parse lines for setting up camera transforms
Camera :: Camera(vector<string> lines) {
  set_position(lines[1]);
  set_orient(lines[2]);
  set_perspective(lines);
}

// Parse position translation line
void Camera :: set_position(string line) {
  istringstream line_stream(line);

  string _;
  float x, y, z;
  if (!(line_stream >> _ >> x >> y >> z)) {
    throw "Wrong number of arguments to camera position line";
  }

  pos = Vertex(x, y, z);
}

// Parse position orientation rotation line
void Camera :: set_orient(string line) {
  istringstream line_stream(line);

  string orientation;
  float x, y, z;
  if (!(line_stream >> orientation >> x >> y >> z >> orient_angle)) {
    throw "Wrong number of arguments to camera orientation line";
  }

  // Normalize orientation axis
  float magnitude = sqrt(x*x + y*y + z*z);
  orient = Vertex(x/magnitude, y/magnitude, z/magnitude);
}

// Get all arguments for perspective projection transform
void Camera :: set_perspective(vector<string> lines) {
  istringstream near_line_stream(lines[3]);
  string _;
  if (!(near_line_stream >> _ >> near)) {
    throw "Wrong number of arguments to camera perspective near line";
  }

  istringstream far_line_stream(lines[4]);
  if (!(far_line_stream >> _ >> far)) {
    throw "Wrong number of arguments to camera perspective far line";
  }

  istringstream left_line_stream(lines[5]);
  if (!(left_line_stream >> _ >> left)) {
    throw "Wrong number of arguments to camera perspective left line";
  }

  istringstream right_line_stream(lines[6]);
  if (!(right_line_stream >> _ >> right)) {
    throw "Wrong number of arguments to camera perspective right line";
  }

  istringstream top_line_stream(lines[7]);
  if (!(top_line_stream >> _ >> top)) {
    throw "Wrong number of arguments to camera perspective top line";
  }

  istringstream bottom_line_stream(lines[8]);
  if (!(bottom_line_stream >> _ >> bottom)) {
    throw "Wrong number of arguments to camera perspective bottom line";
  }
}
