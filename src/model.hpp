#ifndef MODEL_HPP
#define MODEL_HPP

#include <cstddef>
#include <iostream>
#include <memory> // shared_ptr
#include <string>
#include <vector>

#include "halfedge.hpp"
#include "structs.hpp"

using namespace std;

struct Pixel;

using MaterialPtr = shared_ptr<struct Material>;
using ReflectPtr = shared_ptr<struct Reflectance>;

/* The following struct is used for storing a set of transformations.
 * Please note that this structure assumes that our scenes will give
 * sets of transformations in the form of transltion -> rotation -> scaling.
 * Obviously this will not be the case for your scenes. Keep this in
 * mind when writing your own programs.
 *
 * Note that we do not need to use matrices this time to represent the
 * transformations. This is because OpenGL will handle all the matrix
 * operations for us when we have it apply the transformations. All we
 * need to do is supply the parameters.
 */
struct Transforms
{
    /* For each array below,
     * Index 0 has the x-component
     * Index 1 has the y-component
     * Index 2 has the z-component
     */
    float translation[3];
    float rotation[3];
    float scaling[3];

    /* Angle in degrees.
     */
    float rotation_angle;
};

// Represents the reflectance property of a material
struct Reflectance {
  double red, green, blue;

  Reflectance(double r, double g, double b) {
    red = r;
    green = g;
    blue = b;
  }
};

// Represents the material properties of an object
struct Material {
  ReflectPtr ambient, diffuse, specular;
  double shininess;

  Material() {
  }
};

/* The following struct is used to represent objects.
 *
 */
class Model {
  public:
    Mesh_Data *mesh_data;
    string name;
    vector<Vertex> vertices;
    vector<Face> faces;
    MaterialPtr material;

    // Vertices and normals in order by face
    vector<Vertex> vertex_buffer;
    vector<Normal> normal_buffer;

    vector<Transforms> transform_sets;

    /* Index 0 has the r-component
     * Index 1 has the g-component
     * Index 2 has the b-component
     */
    float ambient_reflect[3];
    float diffuse_reflect[3];
    float specular_reflect[3];

    float shininess;

    // empty constructor
    Model();
    // constructor using file
    Model(string raw_file_name);

    // helper function for constructor to get object name
    string get_name(string raw_file_name);
    // helper function for constructor to setup vertices
    void setup_vertices();

    // Set redundant varibles to be used in OpenGL framework
    void set_variables();
};

#endif
