#include "opengl_renderer.hpp"

#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>

#include "arcball.hpp"
#include "camera.hpp"
#include "implicit_fairing.hpp"
#include "model.hpp"
#include "parser.hpp"
#include "structs.hpp"
#include "transform_obj.hpp"

#include "Eigen/Dense"

using namespace std;

void init(void) {
  last_rotation = Eigen::Quaterniond::Identity();
  curr_rotation = Eigen::Quaterniond::Identity();

  // Smooth shading, backface culling, depth buffering
  glShadeModel(GL_SMOOTH);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);

  // Auto normalize normal vectors
  glEnable(GL_NORMALIZE);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  // Set perspective Projection Matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(cam->left, cam->right,
      cam->bottom, cam->top,
      cam->near, cam->far);

  // Transform Modelview Matrix before Projection Matrix transform
  glMatrixMode(GL_MODELVIEW);

  init_lights();
}

void reshape(int width, int height) {
  // Prevent divide by 0 errors, 1x1 smallest window pixel size
  height = (height == 0) ? 1 : height;
  width = (width == 0) ? 1 : width;

  // How to convert NDC to screen coordinates
  glViewport(0, 0, width, height);

  mouse_scale_x = (float) (cam->right - cam->left) / (float) width;
  mouse_scale_y = (float) (cam->top - cam->bottom) / (float) height;

  glutPostRedisplay();
}

void display(void) {
  // Reset color buffer to black and depth buffer to big number
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Reset Modelview Matrix to identity
  glLoadIdentity();
  // Create rotation matrix
  glRotatef(y_view_angle, 1, 0, 0);
  glRotatef(x_view_angle, 0, 1, 0);

  // Camera rotation and translation
  glRotatef(-1.0*rad2deg(cam->orient_angle), cam->orient.x, cam->orient.y, cam->orient.z);
  glTranslatef(-cam->pos.x, -cam->pos.y, -cam->pos.z);

  GLdouble *cam_rotation = get_current_rotation(curr_rotation, last_rotation);
  glMultMatrixd(cam_rotation);
  delete[] cam_rotation;

  set_lights();
  draw_objects();

  // Swap the active and off-screen buffers.
  glutSwapBuffers();
}

void init_lights() {
  // Automatically apply reflection or lighting model to every pixel
  glEnable(GL_LIGHTING);

  int num_lights = lights.size();

  for(int i = 0; i < num_lights; ++i) {
    // Associate each of our point lights with one of OpenGL's built-in lights.
    int light_id = GL_LIGHT0 + i;

    glEnable(light_id);

    // Set the color of the light
    glLightfv(light_id, GL_AMBIENT, lights[i].color);
    glLightfv(light_id, GL_DIFFUSE, lights[i].color);
    glLightfv(light_id, GL_SPECULAR, lights[i].color);

    // Set the attenuation k constant of the light.
    glLightf(light_id, GL_QUADRATIC_ATTENUATION, lights[i].attenuation_k);
  }
}

void set_lights() {
  int num_lights = lights.size();

  for(int i = 0; i < num_lights; ++i) {
    int light_id = GL_LIGHT0 + i;

    glLightfv(light_id, GL_POSITION, lights[i].position);
  }
}

void draw_objects() {
  int num_objects = objects.size();

  for(int i = 0; i < num_objects; ++i) {
    // Keep a copy of Modelview Matrix before modifying
    glPushMatrix();
    {
      int num_transform_sets = 0;

      // Modify Modelview Matrix with object's geo-transforms
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

      // Tell OpenGL material properties of surface
      glMaterialfv(GL_FRONT, GL_AMBIENT, objects[i].ambient_reflect);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, objects[i].diffuse_reflect);
      glMaterialfv(GL_FRONT, GL_SPECULAR, objects[i].specular_reflect);
      glMaterialf(GL_FRONT, GL_SHININESS, objects[i].shininess);

      // Tell OpenGL to render geometry for us
      glVertexPointer(3, GL_FLOAT, 0, &objects[i].vertex_buffer[0]);
      glNormalPointer(GL_FLOAT, 0, &objects[i].normal_buffer[0]);

      int buffer_size = objects[i].vertex_buffer.size();

      if(!wireframe_mode) {
        // Render all faces
        glDrawArrays(GL_TRIANGLES, 0, buffer_size);
      } else {
        // Render lines instead of triangle surfaces
        for(int j = 0; j < buffer_size; j += 3) {
          glDrawArrays(GL_LINE_LOOP, j, 3);
        }
      }
    }
    // Get original matrix without transformation back
    glPopMatrix();
  }
}

void mouse_pressed(int button, int state, int x, int y) {
  // If left mouse clicked down
  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    // Store start coordinates
    p_x_start = x;
    p_y_start = y;

    // Store the mouse position in our global variables.
    mouse_x = x;
    mouse_y = y;

    is_pressed = true;
  }
  // If left mouse released
  else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    last_rotation = curr_rotation * last_rotation;
    curr_rotation = Eigen::Quaterniond::Identity();

    is_pressed = false;
  }
}

void mouse_moved(int x, int y) {
  // If left mouse clicked down
  if(is_pressed) {
    mouse_x = x;
    mouse_y = y;

    curr_rotation = compute_rotation_quaternion(p_x_start, p_y_start, x, y, xres, yres);

    // Tell OpenGL to re-render our scene with the new camera angles
    glutPostRedisplay();
  }
}

void key_pressed(unsigned char key, int x, int y) {
  if(key == 'q') {
    // Quit the program.
    exit(0);
  } else if(key == 't') {
    // Toggle wireframe mode
    wireframe_mode = !wireframe_mode;
    glutPostRedisplay();
  } else if (key == 'f') {
    // Apply implicit_fairing
    cout << "Smoothing image..." << endl;
    implicit_fairing(objects, time_step);
    glutPostRedisplay();
    cout << "Done smoothing" << endl;
  } else {
    float x_view_rad = deg2rad(x_view_angle);

    // 'w' for step forward
    if(key == 'w') {
      cam->pos.x += step_size * sin(x_view_rad);
      cam->pos.z -= step_size * cos(x_view_rad);
      glutPostRedisplay();
    } else if(key == 'a') {
    // 'a' for step left
      cam->pos.x -= step_size * cos(x_view_rad);
      cam->pos.z -= step_size * sin(x_view_rad);
      glutPostRedisplay();
    } else if(key == 's') {
    // 's' for step backward
      cam->pos.x -= step_size * sin(x_view_rad);
      cam->pos.z += step_size * cos(x_view_rad);
      glutPostRedisplay();
    } else if(key == 'd') {
    // 'd' for step right
      cam->pos.x += step_size * cos(x_view_rad);
      cam->pos.z += step_size * sin(x_view_rad);
      glutPostRedisplay();
    }
  }
}

float deg2rad(float angle) {
  return angle * M_PI / 180.0;
}

float rad2deg(float angle) {
  return angle * 180.0 / M_PI;
}

int main(int argc, char* argv[]) {
  // Parse arguments
  if (argc != 5) {
    cerr << "usage: " << argv[0]
         << " [scene_description_file.txt] [xres] [yres] [h]" << endl;
    exit(-1);
  }
  char *file_name = argv[1];
  xres = atoi(argv[2]);
  yres = atoi(argv[3]);
  time_step = atof(argv[4]);

  // Parse camera parameters
  cam = parse_camera_data(file_name);

  // Get lights as vector<Light>
  lights = parse_light_data(file_name);

  // Parse model data and create geo-transformed copies with material props
  objects = store_obj_transform_file(file_name);

  // Initialize GLUT library
  glutInit(&argc, argv);
  // Tell OpenGL we need a double buffer, RGB pixel buffer,and depth buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  glutInitWindowSize(xres, yres);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("HW5");

  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse_pressed);
  glutMotionFunc(mouse_moved);
  glutKeyboardFunc(key_pressed);

  // Keep doing display, reshape, mouse, and keyboard functions
  glutMainLoop();
}
