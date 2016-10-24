#include <iostream>
#include <memory> // shared_ptr
#include <stdlib.h> // atoi
#include <string>
#include <vector>

#include "camera.hpp"
#include "draw_pixels.hpp"
#include "light.hpp"
#include "parser.hpp"
#include "shading.hpp"
#include "three_d_model_transform.hpp"
#include "transform_obj.hpp"

#include "Eigen/Dense"

using namespace std;

using CameraPtr = shared_ptr<Camera>;
using LightPtr = shared_ptr<Light>;
using ThreeDModelPtr = shared_ptr<ThreeDModel>;

using LightVecPtr = shared_ptr<vector<LightPtr>>;
using ModelVectorPtr = shared_ptr<vector<ThreeDModelPtr>>;

int main (int argc, char **argv) {
  if (argc != 5) {
    cerr << "usage: " << argv[0]
         << " [scene_description_file.txt] [xres] [yres] [mode]" << endl;
    exit(-1);
  }

  char *file_name = argv[1];
  int xres = atoi(argv[2]);
  int yres = atoi(argv[3]);
  int mode = atoi(argv[4]);

  CameraPtr cam = parse_camera_data(file_name);
  LightVecPtr lights = parse_light_data(file_name);
  // Parse model data and create geometrically transformed copies with material properties
  ModelVectorPtr models = store_obj_transform_file(file_name);

  // Apply lighting model and rasterize
  Pixel **grid = new_grid(xres, yres);
  if (mode == 0) {
    gouraud(models, lights, cam, xres, yres, grid);
  } else if (mode == 1) {
    phong(models, lights, cam, xres, yres, grid);
  } else {
    cerr << "mode must be 0 or 1" << endl;
  }

  // Print
  output_ppm(xres, yres, grid);

  delete_grid(xres, yres, grid);
  delete[] grid;

  return 0;
}
