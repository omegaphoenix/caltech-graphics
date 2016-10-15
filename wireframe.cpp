#include <iostream>
#include <stdlib.h> // atoi
#include <string>
#include <vector>

#include "three_d_model_transform.hpp"
#include "transform_obj.hpp"

#include "Eigen/Dense"

using namespace std;

int main (int argc, char **argv) {
  if (argc != 4) {
    cerr << "usage: " << argv[0]
         << " [scene_description_file.txt] [xres] [yres]" << endl;
    exit(-1);
  }

  // Store original objects
  vector<shared_ptr<ThreeDModel> > *models = store_obj_transform_file(argv[1]);
  // Create copies, transform, and print
  print_ppm(atoi(argv[2]), atoi(argv[3]), models);

  return 0;
}
