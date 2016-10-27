/* CS/CNS 171
 * Adapted from opengl_demo.cpp written by Kevin (Kevli) Li (Class of 2016)
 * Originally for Fall 2014
 */


/* The following 2 headers contain all the main functions, data structures, and
 * variables that allow for OpenGL development.
 */
#include <GL/glew.h>
#include <GL/glut.h>

/* You will almost always want to include the math library. For those that do
 * not know, the '_USE_MATH_DEFINES' line allows you to use the syntax 'M_PI'
 * to represent pi to double precision in C++. OpenGL works in degrees for
 * angles, so converting between degrees and radians is a common task when
 * working in OpenGL.
 *
 * Besides the use of 'M_PI', the trigometric functions also show up a lot in
 * graphics computations.
 */
#include <math.h>
#define _USE_MATH_DEFINES

// iostream and vector are standard libraries that are just generally useful.
#include <iostream>
#include <vector>

#include "camera.hpp"
#include "light.hpp"
#include "normal.hpp"
#include "parser.hpp"
#include "transform_obj.hpp"
#include "model.hpp"
#include "vertex.hpp"

#include "Eigen/Dense"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

/* The following are function prototypes for the functions that you will most
 * often write when working in OpenGL.
 *
 * Details on the functions will be given in their respective implementations
 * further below.
 */

void init(void);
void reshape(int width, int height);
void display(void);

void init_lights();
void set_lights();
void draw_objects();

void mouse_pressed(int button, int state, int x, int y);
void mouse_moved(int x, int y);
void key_pressed(unsigned char key, int x, int y);

Eigen::Vector3d create_NDC(int x, int y);
Eigen::Quaterniond compute_rotation_quaternion(Eigen::Vector3d v0, Eigen::Vector3d v1);
GLdouble *get_current_rotation();
Eigen::Vector3d get_unit_rotation_axis(Eigen::Vector3d v0, Eigen::Vector3d v1);
double get_rotation_angle(int x0, int y0, int x1, int y1);
double screen_to_NDC(int screen_coord, int coord_res);

///////////////////////////////////////////////////////////////////////////////

/* The following structs do not involve OpenGL, but they are useful ways to
 * store information needed for rendering,
 *
 * After Assignment 2, the 3D shaded surface renderer assignment, you should
 * have a fairly intuitive understanding of what these structs represent.
 */

///////////////////////////////////////////////////////////////////////////////

/* The following are the typical camera specifications and parameters. In
 * general, it is a better idea to keep all this information in a camera
 * struct, like how we have been doing it in Assignemtns 1 and 2. However,
 * if you only have one camera for the scene, and all your code is in one
 * file (like this one), then it is sometimes more convenient to just have
 * all the camera specifications and parameters as global variables.
 */

/* Index 0 has the x-coordinate
 * Index 1 has the y-coordinate
 * Index 2 has the z-coordinate
 */
float cam_position[] = {0.9, 0, 5.4};
float cam_orientation_axis[] = {0, 0, 1};

/* Angle in degrees.
*/
float cam_orientation_angle = 0;

float near_param = 1, far_param = 20,
      left_param = -0.5, right_param = 0.5,
      top_param = 0.5, bottom_param = -0.5;

///////////////////////////////////////////////////////////////////////////////

/* Self-explanatory lists of lights and objects.
*/

vector<Light> lights;
vector<Model> objects;

///////////////////////////////////////////////////////////////////////////////

/* The following are parameters for creating an interactive first-person camera
 * view of the scene. The variables will make more sense when explained in
 * context, so you should just look at the 'mousePressed', 'mouseMoved', and
 * 'keyPressed' functions for the details.
 */

int mouse_x, mouse_y;
float mouse_scale_x, mouse_scale_y;

const float step_size = 0.2;
const float x_view_step = 90.0, y_view_step = 90.0;
float x_view_angle = 0, y_view_angle = 0;

bool is_pressed = false;
bool wireframe_mode = false;

///////////////////////////////////////////////////////////////////////////////

Eigen::Quaterniond last_rotation;
Eigen::Quaterniond curr_rotation;

int p_x_start;
int p_y_start;

int xres;
int yres;

/* From here on are all the function implementations.
*/


/* 'init' function:
 *
 * As you would expect, the 'init' function initializes and sets up the
 * program. It should always be called before anything else.
 *
 * Writing an 'init' function is not required by OpenGL. If you wanted to, you
 * could just put all your initializations in the beginning of the 'main'
 * function instead. However, doing so is bad style; it is cleaner to have all
 * your initializations contained within one function.
 *
 * Before we go into the function itself, it is important to mention that
 * OpenGL works like a state machine. It will do different procedures depending
 * on what state it is in.
 *
 * For instance, OpenGL has different states for its shading procedure. By
 * default, OpenGL is in "flat shading state", meaning it will always use flat
 * shading when we tell it to render anything. With some syntax, we can change
 * the shading procedure from the "flat shading state" to the "Gouraud shading
 * state", and then OpenGL will render everything using Gouraud shading.
 *
 * The most important task of the 'init' function is to set OpenGL to the
 * states that we want it to be in.
 */
void init(void) {
  // Initialize to identity
  last_rotation = Eigen::Quaterniond::Identity();
  curr_rotation = Eigen::Quaterniond::Identity();
  /* The following line of code tells OpenGL to use "smooth shading" (aka
   * Gouraud shading) when rendering.
   *
   * Yes. This is actually all you need to do to use Gouraud shading in
   * OpenGL (besides providing OpenGL the vertices and normals to render).
   * Short and sweet, right?
   *
   * If you wanted to tell OpenGL to use flat shading at any point, then you
   * would use the following line:

   glShadeModel(GL_FLAT);

   * Phong shading unfortunately requires GLSL, so it will be covered in a
   * later demo.
   */
  glShadeModel(GL_SMOOTH);

  /* The next line of code tells OpenGL to use "culling" when rendering. The
   * line right after it tells OpenGL that the particular "culling" technique
   * we want it to use is backface culling.
   *
   * "Culling" is actually a generic term for various algorithms that
   * prevent the rendering process from trying to render unnecessary
   * polygons. Backface culling is the most commonly used method, but
   * there also exist other, niche methods like frontface culling.
   */
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  /* The following line tells OpenGL to use depth buffering when rendering.
  */
  glEnable(GL_DEPTH_TEST);

  /* The following line tells OpenGL to automatically normalize our normal
   * vectors before it passes them into the normal arrays discussed below.
   * This is required for correct lighting, but it also slows down our
   * program. An alternative to this is to manually scale the normal vectors
   * to correct for each scale operation we call. For instance, if we were
   * to scale an object by 3 (via glScalef() discussed below), then
   * OpenGL would scale the normals of the object by 1/3, as we would
   * expect from the inverse normal transform. But since we need unit
   * normals for lighting, we would either need to enable GL_NORMALIZE
   * or manually scale our normals by 3 before passing them into the
   * normal arrays; this is of course to counteract the 1/3 inverse
   * scaling when OpenGL applies the normal transforms. Enabling GL_NORMALIZE
   * is more convenient, but we sometimes don't use it if it slows down
   * our program too much.
   */
  glEnable(GL_NORMALIZE);

  /* The following two lines tell OpenGL to enable its "vertex array" and
   * "normal array" functionality. More details on these arrays are given
   * in the comments on the 'Model' class and the 'draw_objects' and
   * 'create_objects' functions.
   */
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  /* The next 4 lines work with OpenGL's two main matrices: the "Projection
   * Matrix" and the "Modelview Matrix". Only one of these two main matrices
   * can be modified at any given time. We specify the main matrix that we
   * want to modify with the 'glMatrixMode' function.
   *
   * The Projection Matrix is the matrix that OpenGL applies to points in
   * camera space. For our purposes, we want the Projection Matrix to be
   * the perspective projection matrix, since we want to convert points into
   * NDC after they are in camera space.
   *
   * The line of code below:
   */
  glMatrixMode(GL_PROJECTION);
  /* ^tells OpenGL that we are going to modify the Projection Matrix. From
   * this point on, any matrix comamnds we give OpenGL will affect the
   * Projection Matrix. For instance, the line of code below:
   */
  glLoadIdentity();
  /* ^tells OpenGL to set the current main matrix (which is the Projection
   * Matrix right now) to the identity matrix. Then, the next line of code:
   */
  glFrustum(left_param, right_param,
      bottom_param, top_param,
      near_param, far_param);
  /* ^ tells OpenGL to create a perspective projection matrix using the
   * given frustum parameters. OpenGL then post-multiplies the current main
   * matrix (the Projection Matrix) with the created matrix. i.e. let 'P'
   * be our Projection Matrix and 'F' be the matrix created by 'glFrustum'.
   * Then, after 'F' is created, OpenGL performs the following operation:
   *
   * P = P * F
   *
   * Since we had set the Projection Matrix to the identity matrix before the
   * call to 'glFrustum', the above multiplication results in the Projection
   * Matrix being the perspective projection matrix, which is what we want.
   */

  /* The Modelview Matrix is the matrix that OpenGL applies to untransformed
   * points in world space. OpenGL applies the Modelview Matrix to points
   * BEFORE it applies the Projection Matrix.
   *
   * Thus, for our purposes, we want the Modelview Matrix to be the overall
   * transformation matrix that we apply to points in world space before
   * applying the perspective projection matrix. This means we would need to
   * factor in all the individual object transformations and the camera
   * transformations into the Modelview Matrix.
   *
   * The following line of code tells OpenGL that we are going to modify the
   * Modelview Matrix. From this point on, any matrix commands we give OpenGL
   * will affect the Modelview Matrix.
   *
   * We generally modify the Modelview Matrix in the 'display' function,
   * right before we tell OpenGL to render anything. See the 'display'
   * for details.
   */
  glMatrixMode(GL_MODELVIEW);

  /* The next line calls our function that tells OpenGL to initialize some
   * lights to represent our Point Light structs. Further details will be
   * given in the function itself.
   *
   * The reason we have this procedure as a separate function is to make
   * the code more organized.
   */
  init_lights();
}

/* 'reshape' function:
 *
 * You will see down below in the 'main' function that whenever we create a
 * window in OpenGL, we have to specify a function for OpenGL to call whenever
 * the window resizes. We typically call this function 'reshape' or 'resize'.
 *
 * The 'reshape' function is supposed to tell your program how to react
 * whenever the program window is resized. It is also called in the beginning
 * when the window is first created. You can think of the first call to
 * 'reshape' as an initialization phase and all subsequent calls as update
 * phases.
 *
 * Anything that needs to know the dimensions of the program window should
 * be initialized and updated in the 'reshape' function. You will see below
 * that we use the 'reshape' function to initialize and update the conversion
 * scheme between NDC and screen coordinates as well as the mouse interaction
 * parameters.
 */
void reshape(int width, int height) {
  /* The following two lines of code prevent the width and height of the
   * window from ever becoming 0 to prevent divide by 0 errors later.
   * Typically, we let 1x1 square pixel be the smallest size for the window.
   */
  height = (height == 0) ? 1 : height;
  width = (width == 0) ? 1 : width;

  /* The 'glViewport' function tells OpenGL to determine how to convert from
   * NDC to screen coordinates given the dimensions of the window. The
   * parameters for 'glViewport' are (in the following order):
   *
   * - int x: x-coordinate of the lower-left corner of the window in pixels
   * - int y: y-coordinate of the lower-left corner of the window in pixels
   * - int width: width of the window
   * - int height: height of the window
   *
   * We typically just let the lower-left corner be (0,0).
   *
   * After 'glViewport' is called, OpenGL will automatically know how to
   * convert all our points from NDC to screen coordinates when it tries
   * to render them.
   */
  glViewport(0, 0, width, height);

  /* The following two lines are specific to updating our mouse interface
   * parameters. Details will be given in the 'mouse_moved' function.
   */
  mouse_scale_x = (float) (right_param - left_param) / (float) width;
  mouse_scale_y = (float) (top_param - bottom_param) / (float) height;

  /* The following line tells OpenGL that our program window needs to
   * be re-displayed, meaning everything that was being displayed on
   * the window before it got resized needs to be re-rendered.
   */
  glutPostRedisplay();
}

/* 'display' function:
 *
 * You will see down below in the 'main' function that whenever we create a
 * window in OpenGL, we have to specify a function for OpenGL to call whenever
 * it wants to render anything. We typically name this function 'display' or
 * 'render'.
 *
 * The 'display' function is supposed to handle all the processing of points
 * in world and camera space.
 */
void display(void) {
  /* The following line of code is typically the first line of code in any
   * 'display' function. It tells OpenGL to reset the "color buffer" (which
   * is our pixel grid of RGB values) and the depth buffer.
   *
   * Resetting the "color buffer" is equivalent to clearing the program
   * window so that it only displays a black background. This allows OpenGL
   * to render a new scene onto the window without having to deal with the
   * remnants of the previous scene.
   *
   * Resetting the depth buffer sets all the values in the depth buffer back
   * to a very high number. This allows the depth buffer to be reused for
   * rendering a new scene.
   */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* With the program window cleared, OpenGL is ready to render a new scene.
   * Of course, before we can render anything correctly, we need to make all
   * the appropriate camera and object transformations to our coordinate
   * space.
   *
   * Recall that the 'init' function used the glMatrixMode function to put
   * OpenGL into a state where we can modify its Modelview Matrix. Also
   * recall that we want the Modelview Matrix to be the overall transform-
   * ation matrix that we apply to points in world space before applying the
   * perspective projection matrix. This means that we need to factor in all
   * the individual object transformations and the camera transformations
   * into the Modelview Matrix.
   *
   * To do so, our first step is to "reset" the Modelview Matrix by setting it
   * to the identity matrix:
   */
  glLoadIdentity();
  /* Now, if you recall, for a given object, we want to FIRST multiply the
   * coordinates of its points by the translations, rotations, and scalings
   * applied to the object and THEN multiply by the inverse camera rotations
   * and translations.
   *
   * HOWEVER, OpenGL modifies the Modelview Matrix using POST-MULTIPLICATION.
   * This means that if were to specify to OpenGL a matrix modification 'A',
   * then letting the Modelview Matrix be 'M', OpenGL would perform the
   * following operation:
   *
   * M = M * A
   *
   * So, for instance, if the Modelview Matrix were initialized to the
   * identity matrix 'I' and we were to specify a translation 'T' followed by
   * a rotation 'R' followed by a scaling 'S' followed by the inverse camera
   * transform 'C', then the Modelview Matrix is modified in the following
   * order:
   *
   * M = I * T * R * S * C
   *
   * Then, when OpenGL applies the Modelview Matrix to a point 'p', we would
   * get the following multiplication:
   *
   * M * p = I * T * R * S * C * p
   *
   * ^ So the camera transformation ends up being applied first even though
   * it was specified last. This is not what we want. What we want is
   * something like this:
   *
   * M * p = C * T * R * S * I * p
   *
   * Hence, to correctly transform a point, we actually need to FIRST specify
   * the inverse camera rotations and translations and THEN specify the
   * translations, rotations, and scalings applied to an object.
   *
   * We start by specifying any camera rotations caused by the mouse. We do
   * so by using the 'glRotatef' function, which takes the following parameters
   * in the following order:
   *
   * - float angle: rotation angle in DEGREES
   * - float x: x-component of rotation axis
   * - float y: y-component of rotation axis
   * - float z: z-component of rotation axis
   *
   * The 'glRotatef' function tells OpenGL to create a rotation matrix using
   * the given angle and rotation axis.
   */
  glRotatef(y_view_angle, 1, 0, 0);
  glRotatef(x_view_angle, 0, 1, 0);
  /* 'y_view_angle' and 'x_view_angle' are parameters for our mouse user
   * interface. They keep track of how much the user wants to rotate the
   * camera from its default, specified orientation. See the 'mouse_moved'
   * function for more details.
   *
   * Our next step is to specify the inverse rotation of the camera by its
   * orientation angle about its orientation axis:
   */
  glRotatef(-cam_orientation_angle,
      cam_orientation_axis[0], cam_orientation_axis[1], cam_orientation_axis[2]);
  /* We then specify the inverse translation of the camera by its position using
   * the 'glTranslatef' function, which takes the following parameters in the
   * following order:
   *
   * - float x: x-component of translation vector
   * - float y: x-component of translation vector
   * - float z: x-component of translation vector
   */
  glTranslatef(-cam_position[0], -cam_position[1], -cam_position[2]);
  /* ^ And that should be it for the camera transformations.
  */

  GLdouble *cam_rotation = get_current_rotation();
  glMultMatrixd(cam_rotation);
  delete[] cam_rotation;

  /* Our next step is to set up all the lights in their specified positions.
   * Our helper function, 'set_lights' does this for us. See the function
   * for more details.
   *
   * The reason we have this procedure as a separate function is to make
   * the code more organized.
   */
  set_lights();
  /* Once the lights are set, we can specify the points and faces that we
   * want drawn. We do all this in our 'draw_objects' helper function. See
   * the function for more details.
   *
   * The reason we have this procedure as a separate function is to make
   * the code more organized.
   */
  draw_objects();

  /* The following line of code has OpenGL do what is known as "double
   * buffering".
   *
   * Imagine this: You have a relatively slow computer that is telling OpenGL
   * to render something to display in the program window. Because your
   * computer is slow, OpenGL ends up rendering only part of the scene before
   * it displays it in the program window. The rest of the scene shows up a
   * second later. This effect is referred to as "flickering". You have most
   * likely experienced this sometime in your life when using a computer. It
   * is not the most visually appealing experience, right?
   *
   * To avoid the above situation, we need to tell OpenGL to display the
   * entire scene at once rather than rendering the scene one pixel at a
   * time. We do so by enabling "double buffering".
   *
   * Basically, double buffering is a technique where rendering is done using
   * two pixel grids of RGB values. One pixel grid is designated as the
   * "active buffer" while the other is designated as the "off-screen buffer".
   * Rendering is done on the off-screen buffer while the active buffer is
   * being displayed. Once the scene is fully rendered on the off-screen buffer,
   * the two buffers switch places so that the off-screen buffer becomes the
   * new active buffer and gets displayed while the old active buffer becomes
   * the new off-screen buffer. This process allows scenes to be fully rendered
   * onto the screen at once, avoiding the flickering effect.
   *
   * We actually enable double buffering in the 'main' function with the line:

   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

   * ^ 'GLUT_DOUBLE' tells OpenGL to use double buffering. The other two
   * parameters, 'GLUT_RGB' and 'GLUT_DEPTH', tell OpenGL to initialize the
   * RGB pixel grids and the depth buffer respectively.
   *
   * The following function, 'glutSwapBuffers', tells OpenGL to swap the
   * active and off-screen buffers.
   */
  glutSwapBuffers();
}

/* 'init_lights' function:
 *
 * This function has OpenGL enable its built-in lights to represent our point
 * lights.
 *
 * OpenGL has 8 built-in lights in all, each one with its own unique, integer
 * ID value. When setting the properties of a light, we need to tell OpenGL
 * the ID value of the light we are modifying.
 *
 * The first light's ID value is stored in 'GL_LIGHT0'. The second light's ID
 * value is stored in 'GL_LIGHT1'. And so on. The eighth and last light's ID
 * value is stored in 'GL_LIGHT7'.
 *
 * The properties of the lights are set using the 'glLightfv' and 'glLightf'
 * functions as you will see below.
 */
void init_lights() {
  /* The following line of code tells OpenGL to enable lighting calculations
   * during its rendering process. This tells it to automatically apply the
   * Phong reflection model or lighting model to every pixel it will render.
   */
  glEnable(GL_LIGHTING);

  int num_lights = lights.size();

  for(int i = 0; i < num_lights; ++i) {
    /* In this loop, we are going to associate each of our point lights
     * with one of OpenGL's built-in lights. The simplest way to do this
     * is to just let our first point light correspond to 'GL_LIGHT0', our
     * second point light correspond to 'GL_LIGHT1', and so on. i.e. let:
     *
     * 'lights[0]' have an ID value of 'GL_LIGHT0'
     * 'lights[1]' have an ID value of 'GL_LIGHT1'
     * etc...
     */
    int light_id = GL_LIGHT0 + i;

    glEnable(light_id);

    /* The following lines of code use 'glLightfv' to set the color of
     * the light. The parameters for 'glLightfv' are:
     *
     * - enum light_ID: an integer between 'GL_LIGHT0' and 'GL_LIGHT7'
     * - enum property: this varies depending on what you are setting
     *                  e.g. 'GL_AMBIENT' for the light's ambient component
     * - float* values: a set of values to set for the specified property
     *                  e.g. an array of RGB values for the light's color
     *
     * OpenGL actually lets us specify different colors for the ambient,
     * diffuse, and specular components of the light. However, since we
     * are used to only working with one overall light color, we will
     * just set every component to the light color.
     */
    glLightfv(light_id, GL_AMBIENT, lights[i].color);
    glLightfv(light_id, GL_DIFFUSE, lights[i].color);
    glLightfv(light_id, GL_SPECULAR, lights[i].color);

    /* The following line of code sets the attenuation k constant of the
     * light. The difference between 'glLightf' and 'glLightfv' is that
     * 'glLightf' is used for when the parameter is only one value like
     * the attenuation constant while 'glLightfv' is used for when the
     * parameter is a set of values like a color array. i.e. the third
     * parameter of 'glLightf' is just a float instead of a float*.
     */
    glLightf(light_id, GL_QUADRATIC_ATTENUATION, lights[i].attenuation_k);
  }
}

/* 'set_lights' function:
 *
 * While the 'init_lights' function enables and sets the colors of the lights,
 * the 'set_lights' function is supposed to position the lights.
 *
 * You might be wondering why we do not just set the positions of the lights in
 * the 'init_lights' function in addition to the other properties. The reason
 * for this is because OpenGL does lighting computations after it applies the
 * Modelview Matrix to points. This means that the lighting computations are
 * effectively done in camera space. Hence, to ensure that we get the correct
 * lighting computations, we need to make sure that we position the lights
 * correctly in camera space.
 *
 * Now, the 'glLightfv' function, when used to position a light, applies all
 * the current Modelview Matrix to the given light position. This means that
 * to correctly position lights in camera space, we should call the 'glLightfv'
 * function to position them AFTER the Modelview Matrix has been modified by
 * the necessary camera transformations. As you can see in the 'display'
 * function, this is exactly what we do.
 */
void set_lights() {
  int num_lights = lights.size();

  for(int i = 0; i < num_lights; ++i) {
    int light_id = GL_LIGHT0 + i;

    glLightfv(light_id, GL_POSITION, lights[i].position);
  }
}

/* 'draw_objects' function:
 *
 * This function has OpenGL render our objects to the display screen. It
 */
void draw_objects() {
  int num_objects = objects.size();

  for(int i = 0; i < num_objects; ++i) {
    /* The current Modelview Matrix is actually stored at the top of a
     * stack in OpenGL. The following function, 'glPushMatrix', pushes
     * another copy of the current Modelview Matrix onto the top of the
     * stack. This results in the top two matrices on the stack both being
     * the current Modelview Matrix. Let us call the copy on top 'M1' and
     * the copy that is below it 'M2'.
     *
     * The reason we want to use 'glPushMatrix' is because we need to
     * modify the Modelview Matrix differently for each object we need to
     * render, since each object is affected by different transformations.
     * We use 'glPushMatrix' to essentially keep a copy of the Modelview
     * Matrix before it is modified by an object's transformations. This
     * copy is our 'M2'. We then modify 'M1' and use it to render the
     * object. After we finish rendering the object, we will pop 'M1' off
     * the stack with the 'glPopMatrix' function so that 'M2' returns to
     * the top of the stack. This way, we have the old unmodified Modelview
     * Matrix back to edit for the next object we want to render.
     */
    glPushMatrix();
    /* The following brace is not necessary, but it keeps things organized.
    */
    {
      int num_transform_sets = 0;
      // int num_transform_sets = objects[i].transform_sets.size();

      /* The loop tells OpenGL to modify our modelview matrix with the
       * desired geometric transformations for this object. Remember
       * though that our 'transform_sets' struct assumes that transformations
       * are conveniently given in sets of translation -> rotate -> scaling;
       * and THIS IS NOT THE CASE FOR YOUR SCENE FILES. DO NOT BLINDLY
       * COPY THE FOLLOWING CODE.
       *
       * To explain how to correctly transform your objects, consider the
       * following example. Suppose our object has the following desired
       * transformations:
       *
       *    scale by 2, 2, 2
       *    translate by 2, 0, 5
       *    rotate about 0, 1, 0.5 and by angle = 0.6 radians
       *
       * Obviously, we cannot use the following loop for this, because the order
       * is not translate -> rotate -> scale. Instead, we need to make the following
       * calls in this exact order:
       *
       *    glRotatef(0.6 * 180.0 / M_PI, 0, 1, 0.5);
       *    glTranslatef(2, 0, 5);
       *    glScalef(2, 2, 2);
       *
       * We make the calls in the REVERSE order of how the transformations are specified
       * because OpenGL edits our modelview matrix using post-multiplication (see above
       * at the notes regarding the camera transforms in display()).
       *
       * Keep all this in mind to come up with an appropriate way to store and apply
       * geometric transformations for each object in your scenes.
       */
      for(int j = 0; j < num_transform_sets; ++j) {
        glTranslatef(objects[i].transform_sets[j].translation[0],
            objects[i].transform_sets[j].translation[1],
            objects[i].transform_sets[j].translation[2]);
        glRotatef(objects[i].transform_sets[j].rotation_angle,
            objects[i].transform_sets[j].rotation[0],
            objects[i].transform_sets[j].rotation[1],
            objects[i].transform_sets[j].rotation[2]);
        glScalef(objects[i].transform_sets[j].scaling[0],
            objects[i].transform_sets[j].scaling[1],
            objects[i].transform_sets[j].scaling[2]);
      }

      /* The 'glMaterialfv' and 'glMaterialf' functions tell OpenGL
       * the material properties of the surface we want to render.
       * The parameters for 'glMaterialfv' are (in the following order):
       *
       * - enum face: Options are 'GL_FRONT' for front-face rendering,
       *              'GL_BACK' for back-face rendering, and
       *              'GL_FRONT_AND_BACK' for rendering both sides.
       * - enum property: this varies on what you are setting up
       *                  e.g. 'GL_AMBIENT' for ambient reflectance
       * - float* values: a set of values for the specified property
       *                  e.g. an array of RGB values for the reflectance
       *
       * The 'glMaterialf' function is the same, except the third
       * parameter is only a single float value instead of an array of
       * values. 'glMaterialf' is used to set the shininess property.
       */
      glMaterialfv(GL_FRONT, GL_AMBIENT, objects[i].ambient_reflect);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, objects[i].diffuse_reflect);
      glMaterialfv(GL_FRONT, GL_SPECULAR, objects[i].specular_reflect);
      glMaterialf(GL_FRONT, GL_SHININESS, objects[i].shininess);

      /* The next few lines of code are how we tell OpenGL to render
       * geometry for us. First, let us look at the 'glVertexPointer'
       * function.
       *
       * 'glVertexPointer' tells OpenGL the specifications for our
       * "vertex array". As a recap of the comments from the 'Object'
       * struct, the "vertex array" stores all the faces of the surface
       * we want to render. The faces are stored in the array as
       * consecutive points. For instance, if our surface were a cube,
       * then our "vertex array" could be the following:
       *
       * [face1vertex1, face1vertex2, face1vertex3, face1vertex4,
       *  face2vertex1, face2vertex2, face2vertex3, face2vertex4,
       *  face3vertex1, face3vertex2, face3vertex3, face3vertex4,
       *  face4vertex1, face4vertex2, face4vertex3, face4vertex4,
       *  face5vertex1, face5vertex2, face5vertex3, face5vertex4,
       *  face6vertex1, face6vertex2, face6vertex3, face6vertex4]
       *
       * Obviously to us, some of the vertices in the array are repeats.
       * However, the repeats cannot be avoided since OpenGL requires
       * this explicit specification of the faces.
       *
       * The parameters to the 'glVertexPointer' function are as
       * follows:
       *
       * - int num_points_per_face: this is the parameter that tells
       *                            OpenGL where the breaks between
       *                            faces are in the vertex array.
       *                            Below, we set this parameter to 3,
       *                            which tells OpenGL to treat every
       *                            set of 3 consecutive vertices in
       *                            the vertex array as 1 face. So
       *                            here, our vertex array is an array
       *                            of triangle faces.
       *                            If we were using the example vertex
       *                            array above, we would have set this
       *                            parameter to 4 instead of 3.
       * - enum type_of_coordinates: this parameter tells OpenGL whether
       *                             our vertex coordinates are ints,
       *                             floats, doubles, etc. In our case,
       *                             we are using floats, hence 'GL_FLOAT'.
       * - sizei stride: this parameter specifies the number of bytes
       *                 between consecutive vertices in the array.
       *                 Most often, you will set this parameter to 0
       *                 (i.e. no offset between consecutive vertices).
       * - void* pointer_to_array: this parameter is the pointer to
       *                           our vertex array.
       */
      glVertexPointer(3, GL_FLOAT, 0, &objects[i].vertex_buffer[0]);
      /* The "normal array" is the equivalent array for normals.
       * Each normal in the normal array corresponds to the vertex
       * of the same index in the vertex array.
       *
       * The 'glNormalPointer' function has the following parameters:
       *
       * - enum type_of_normals: e.g. int, float, double, etc
       * - sizei stride: same as the stride parameter in 'glVertexPointer'
       * - void* pointer_to_array: the pointer to the normal array
       */
      glNormalPointer(GL_FLOAT, 0, &objects[i].normal_buffer[0]);

      int buffer_size = objects[i].vertex_buffer.size();

      if(!wireframe_mode)
        /* Finally, we tell OpenGL to render everything with the
         * 'glDrawArrays' function. The parameters are:
         *
         * - enum mode: in our case, we want to render triangles,
         *              so we specify 'GL_TRIANGLES'. If we wanted
         *              to render squares, then we would use
         *              'GL_QUADS' (for quadrilaterals).
         * - int start_index: the index of the first vertex
         *                    we want to render in our array
         * - int num_vertices: number of vertices to render
         *
         * As OpenGL renders all the faces, it automatically takes
         * into account all the specifications we have given it to
         * do all the lighting calculations for us. It also applies
         * the Modelview and Projection matrix transformations to
         * the vertices and converts everything to screen coordinates
         * using our Viewport specification. Everything is rendered
         * onto the off-screen buffer.
         */
        glDrawArrays(GL_TRIANGLES, 0, buffer_size);
      else
        /* If we are in "wireframe mode" (see the 'key_pressed'
         * function for more information), then we want to render
         * lines instead of triangle surfaces. To render lines,
         * we use the 'GL_LINE_LOOP' enum for the mode parameter.
         * However, we need to draw each face frame one at a time
         * to render the wireframe correctly. We can do so with a
         * for loop:
         */
        for(int j = 0; j < buffer_size; j += 3)
          glDrawArrays(GL_LINE_LOOP, j, 3);
    }
    /* As discussed before, we use 'glPopMatrix' to get back the
     * version of the Modelview Matrix that we had before we specified
     * the object transformations above. We then move on in our loop
     * to the next object we want to render.
     */
    glPopMatrix();
  }
}

Eigen::Vector3d create_NDC(int x, int y) {
  double NDC_x = screen_to_NDC(x, xres);
  double NDC_y = -screen_to_NDC(y, yres);
  double NDC_z_squared = 1 - NDC_x*NDC_x - NDC_y*NDC_y;
  double NDC_z = (NDC_z > 0) ? sqrt(NDC_z_squared) : 0;

  Eigen::Vector3d ver(NDC_x, NDC_y, NDC_z);
  return ver;
}

Eigen::Vector3d get_unit_rotation_axis(Eigen::Vector3d v0, Eigen::Vector3d v1) {
  Eigen::Vector3d unit_rotation_axis = v0.cross(v1);
  if (unit_rotation_axis.norm() != 0)
    unit_rotation_axis.normalize();
  return unit_rotation_axis;
}

double get_rotation_angle(Eigen::Vector3d v0, Eigen::Vector3d v1) {
  double rot_ang_help = v0.dot(v1) / v0.norm() * v1.norm();
  double theta = acos(min(1.0, rot_ang_help));
  return theta;
}

double screen_to_NDC(int screen_coord, int screen_res) {
  double NDC = (2.0 * screen_coord / screen_res) - 1.0;
  return NDC;
}

Eigen::Quaterniond compute_rotation_quaternion(int x0, int y0, int x1, int y1) {
  Eigen::Vector3d v0 = create_NDC(x0, y0);
  Eigen::Vector3d v1 = create_NDC(x1, y1);

  double theta = get_rotation_angle(v0, v1);
  Eigen::Vector3d rotation_axis = get_unit_rotation_axis(v0, v1);

  double quaternion_real = cos(theta/2.0);
  double imaginary_scalar = sin(theta/2.0);

  Eigen::Vector3d quaternion_imaginary = rotation_axis * imaginary_scalar;
  Eigen::Quaterniond q(quaternion_real, quaternion_imaginary(0, 0),
      quaternion_imaginary(1, 0), quaternion_imaginary(2, 0));
  return q;
}

/* 'get_current_rotation' function:
 * Return current rotation as GLdouble array
 */
GLdouble *get_current_rotation() {
  Eigen::Quaterniond res = curr_rotation * last_rotation;
  double x = res.x();
  double y = res.y();
  double z = res.z();
  double s = res.w();

  Eigen::MatrixXd m(4, 4);
  m << 1 - 2*y*y - 2*z*z, 2*(x*y - z*s), 2*(x*z + y*s), 0,
       2*(x*y + z*s), 1 - 2*x*x - 2*z*z, 2*(y*z - x*s), 0,
       2*(x*z - y*s), 2*(y*z + x*s), 1 - 2*x*x - 2*y*y, 0,
			 0, 0, 0, 1;

  // opengl's matrix is column-major
  GLdouble *out = new GLdouble[16];
  int k = 0;
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      out[k] = m(col, row);
      k++;
    }
  }
  return out;
}

/* 'mouse_pressed' function:
 *
 * This function is meant to respond to mouse clicks and releases. The
 * parameters are:
 *
 * - int button: the button on the mouse that got clicked or released,
 *               represented by an enum
 * - int state: either 'GLUT_DOWN' or 'GLUT_UP' for specifying whether the
 *              button was pressed down or released up respectively
 * - int x: the x screen coordinate of where the mouse was clicked or released
 * - int y: the y screen coordinate of where the mouse was clicked or released
 *
 * The function doesn't really do too much besides set some variables that
 * we need for the 'mouse_moved' function.
 */
void mouse_pressed(int button, int state, int x, int y) {
  /* If the left-mouse button was clicked down, then...
  */
  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    // store start coordinates
    p_x_start = x;
    p_y_start = y;
    /* Store the mouse position in our global variables.
    */
    mouse_x = x;
    mouse_y = y;

    /* Since the mouse is being pressed down, we set our 'is_pressed"
     * boolean indicator to true.
     */
    is_pressed = true;
  }
  /* If the left-mouse button was released up, then...
  */
  else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    last_rotation = curr_rotation * last_rotation;
    curr_rotation = Eigen::Quaterniond::Identity();
    /* Mouse is no longer being pressed, so set our indicator to false.
    */
    is_pressed = false;
  }
}

/* 'mouse_moved' function:
 *
 * This function is meant to respond to when the mouse is being moved. There
 * are just two parameters to this function:
 *
 * - int x: the x screen coordinate of where the mouse was clicked or released
 * - int y: the y screen coordinate of where the mouse was clicked or released
 *
 * We compute our Arcball rotation based on the mouse movement in this
 * function.
 */
void mouse_moved(int x, int y) {
  /* If the left-mouse button is being clicked down...
  */
  if(is_pressed) {
    /* We update our 'mouse_x' and 'mouse_y' variables so that if the user moves
     * the mouse again without releasing it, then the distance we compute on the
     * next call to the 'mouse_moved' function will be from this current mouse
     * position.
     */
    mouse_x = x;
    mouse_y = y;

    curr_rotation = compute_rotation_quaternion(x, y, p_x_start, p_y_start);

    /* Tell OpenGL that it needs to re-render our scene with the new camera
     * angles.
     */
    glutPostRedisplay();
  }
}

/* 'deg2rad' function:
 *
 * Converts given angle in degrees to radians.
 */
float deg2rad(float angle) {
  return angle * M_PI / 180.0;
}

/* 'key_pressed' function:
 *
 * This function is meant to respond to key pressed on the keyboard. The
 * parameters are:
 *
 * - unsigned char key: the character of the key itself or the ASCII value of
 *                      of the key
 * - int x: the x screen coordinate of where the mouse was when the key was pressed
 * - int y: the y screen coordinate of where the mouse was when the key was pressed
 *
 * Our function is pretty straightforward as you can see below. We also do not make
 * use of the 'x' and 'y' parameters.
 */
void key_pressed(unsigned char key, int x, int y) {
  // If 'q' is pressed, quit the program.
  if(key == 'q') {
    exit(0);
  }
  /* If 't' is pressed, toggle our 'wireframe_mode' boolean to make OpenGL
   * render our cubes as surfaces of wireframes.
   */
  else if(key == 't') {
    wireframe_mode = !wireframe_mode;
    /* Tell OpenGL that it needs to re-render our scene with the cubes
     * now as wireframes (or surfaces if they were wireframes before).
     */
    glutPostRedisplay();
  }
  else
  {
    /* These might look a bit complicated, but all we are really doing is
     * using our current change in the horizontal camera angle (ie. the
     * value of 'x_view_angle') to compute the correct changes in our x and
     * z coordinates in camera space as we move forward, backward, to the left,
     * or to the right.
     *
     * 'step_size' is an arbitrary value to determine how "big" our steps
     * are.
     *
     * We make the x and z coordinate changes to the camera position, since
     * moving forward, backward, etc is basically just shifting our view
     * of the scene.
     */

    float x_view_rad = deg2rad(x_view_angle);

    // 'w' for step forward
    if(key == 'w') {
      cam_position[0] += step_size * sin(x_view_rad);
      cam_position[2] -= step_size * cos(x_view_rad);
      glutPostRedisplay();
    }
    // 'a' for step left
    else if(key == 'a') {
      cam_position[0] -= step_size * cos(x_view_rad);
      cam_position[2] -= step_size * sin(x_view_rad);
      glutPostRedisplay();
    }
    // 's' for step backward
    else if(key == 's') {
      cam_position[0] -= step_size * sin(x_view_rad);
      cam_position[2] += step_size * cos(x_view_rad);
      glutPostRedisplay();
    }
    // 'd' for step right
    else if(key == 'd') {
      cam_position[0] += step_size * cos(x_view_rad);
      cam_position[2] += step_size * sin(x_view_rad);
      glutPostRedisplay();
    }
  }
}

/* 'main' function:
 */
int main(int argc, char* argv[]) {
  if (argc != 4) {
    cerr << "usage: " << argv[0]
         << " [scene_description_file.txt] [xres] [yres]" << endl;
    exit(-1);
  }
  char *file_name = argv[1];
  xres = atoi(argv[2]);
  yres = atoi(argv[3]);

  // Set camera position and orientation variables
  CameraPtr cam = parse_camera_data(file_name);
  cam_position[0] = cam->pos.x;
  cam_position[1] = cam->pos.y;
  cam_position[2] = cam->pos.z;
  cam_orientation_axis[0] = cam->orient.x;
  cam_orientation_axis[1] = cam->orient.y;
  cam_orientation_axis[2] = cam->orient.z;
  // Convert to degrees
  cam_orientation_angle = cam->orient_angle*180.0 / M_PI;

  // Get lights as vector<Light>
  lights = parse_light_data(file_name);

  // Parse model data and create geometrically transformed copies with material properties
  objects = store_obj_transform_file(file_name);

  /* 'glutInit' intializes the GLUT (Graphics Library Utility Toolkit) library.
   * This is necessary, since a lot of the functions we used above and below
   * are from the GLUT library.
   *
   * 'glutInit' takes the 'main' function arguments as parameters. This is not
   * too important for us, but it is possible to give command line specifications
   * to 'glutInit' by putting them with the 'main' function arguments.
   */
  glutInit(&argc, argv);
  /* The following line of code tells OpenGL that we need a double buffer,
   * a RGB pixel buffer, and a depth buffer.
   */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  /* The following line tells OpenGL to create a program window of size
   * 'xres' by 'yres'.
   */
  glutInitWindowSize(xres, yres);
  /* The following line tells OpenGL to set the program window in the top-left
   * corner of the computer screen (0, 0).
   */
  glutInitWindowPosition(0, 0);
  // The following line tells OpenGL to name the program window "HW3".
  glutCreateWindow("HW3");

  init();
  // Specify to OpenGL our display function.
  glutDisplayFunc(display);
  // Specify to OpenGL our reshape function.
  glutReshapeFunc(reshape);
  // Specify to OpenGL our function for handling mouse presses.
  glutMouseFunc(mouse_pressed);
  // Specify to OpenGL our function for handling mouse movement.
  glutMotionFunc(mouse_moved);
  // Specify to OpenGL our function for handling key presses.
  glutKeyboardFunc(key_pressed);
  /* The following line tells OpenGL to start the "event processing loop". This
   * is an infinite loop where OpenGL will continuously use our display, reshape,
   * mouse, and keyboard functions to essentially run our program.
   */
  glutMainLoop();
}
