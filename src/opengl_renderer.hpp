#ifndef OPENGL_RENDERER_HPP
#define OPENGL_RENDERER_HPP

// Adapted from opengl_demo.cpp written by Kevin (Kevli) Li (Class of 2016)

#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>

#include "arcball.hpp"
#include "camera.hpp"
#include "parser.hpp"
#include "transform_obj.hpp"
#include "model.hpp"
#include "structs.hpp"

#include "Eigen/Dense"

using namespace std;

// Set OpenGL to the states we want it to be in.
void init(void);
// Resize program window
void reshape(int width, int height);
// Render window
void display(void);

// Enable OpenGL built-in lights to represent our point lights
void init_lights();
// Position lights
void set_lights();
// This function has OpenGL render our objects to the display screen.
void draw_objects();

// Respond to mouse clicks and releases
void mouse_pressed(int button, int state, int x, int y);
// Respond when the mouse is being moved.
void mouse_moved(int x, int y);
// Respond to key pressed on the keyboard.
void key_pressed(unsigned char key, int x, int y);

// Convert between degrees and radians
float deg2rad(float angle);
float rad2deg(float angle);

int main(int argc, char* argv[]);

int xres, yres;

Eigen::Quaterniond last_rotation;
Eigen::Quaterniond curr_rotation;

int p_x_start;
int p_y_start;

CameraPtr cam;

vector<Light> lights;
vector<Model> objects;

int mouse_x, mouse_y;
float mouse_scale_x, mouse_scale_y;

const float step_size = 0.2;
const float x_view_step = 90.0, y_view_step = 90.0;
float x_view_angle = 0, y_view_angle = 0;

bool is_pressed = false;
bool wireframe_mode = false;

#endif
