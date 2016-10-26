#ifndef MODEL_HPP
#define MODEL_HPP

#include <cstddef>
#include <iostream>
#include <memory> // shared_ptr
#include <string>
#include <vector>

#include "draw_pixels.hpp"
#include "face.hpp"
#include "normal.hpp"
#include "vertex.hpp"

using namespace std;

struct Pixel;

using FacePtr = shared_ptr<Face>;
using MaterialPtr = shared_ptr<struct Material>;
using NormalPtr = shared_ptr<Normal>;
using ReflectPtr = shared_ptr<struct Reflectance>;

using NormVectorPtr = shared_ptr<vector<NormalPtr>>;

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
 * The main things to note here are the 'vertex_buffer' and 'normal_buffer'
 * vectors.
 *
 * You will see later in the 'draw_objects' function that OpenGL requires
 * us to supply it all the faces that make up an object in one giant
 * "vertex array" before it can render the object. The faces are each specified
 * by the set of vertices that make up the face, and the giant "vertex array"
 * stores all these sets of vertices consecutively. Our "vertex_buffer" vector
 * below will be our "vertex array" for the object.
 *
 * As an example, let's say that we have a cube object. A cube has 6 faces,
 * each with 4 vertices. Each face is going to be represented by the 4 vertices
 * that make it up. We are going to put each of these 4-vertex-sets one by one
 * into 1 large array. This gives us an array of 36 vertices. e.g.:
 *
 * [face1vertex1, face1vertex2, face1vertex3, face1vertex4,
 *  face2vertex1, face2vertex2, face2vertex3, face2vertex4,
 *  face3vertex1, face3vertex2, face3vertex3, face3vertex4,
 *  face4vertex1, face4vertex2, face4vertex3, face4vertex4,
 *  face5vertex1, face5vertex2, face5vertex3, face5vertex4,
 *  face6vertex1, face6vertex2, face6vertex3, face6vertex4]
 *
 * This array of 36 vertices becomes our 'vertex_array'.
 *
 * While it may be obvious to us that some of the vertices in the array are
 * repeats, OpenGL has no way of knowing this. The redundancy is necessary
 * since OpenGL needs the vertices of every face to be explicitly given.
 *
 * The 'normal_buffer' stores all the normals corresponding to the vertices
 * in the 'vertex_buffer'. With the cube example, since the "vertex array"
 * has "36" vertices, the "normal array" also has "36" normals.
 */
class Model {
  public:
    string name;
    shared_ptr<vector<FacePtr>> faces;
    MaterialPtr material;

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
    // helper function for constructor to setup normals
    void setup_normals();
    // Draw model on the grid representing the screen
    void draw_model(int xres, int yres, Pixel **grid);
    // Draw a single face on the grid representing the screen
    void draw_face(int xres, int yres, FacePtr face, Pixel **grid);

    // Set redundant varibles to be used in OpenGL framework
    void set_variables();
};

#endif
