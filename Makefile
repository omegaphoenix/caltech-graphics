###############################################################################
# CS/CNS 171 Fall 2015
#
# This is a template Makefile for the assignment. Edit it however you find
# convenient.
###############################################################################
CC := g++
CFLAGS := -g -std=c++11

SRCDIR := src
BUILDDIR := build
TARGET = bin/opengl_demo

# The following line is a relative directory reference that assumes the Eigen
# folder--which your program will depend on--is located one directory above the
# directory that contains this Makefile.
SRCEXT := cpp
SOURCES:=$(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
INC := -I include -I/usr/X11R6/include -I/usr/include/GL -I/usr/include
LIBS = -lGLEW -lGL -lGLU -lglut -lm
LIBDIR = -L/usr/X11R6/lib -L/usr/local/lib

all: $(TARGET)
	./$(TARGET) data/scene_cube1.txt 800 800 | display

cube1: $(TARGET)
	./$(TARGET) data/scene_cube1.txt 800 800

cube2: $(TARGET)
	./$(TARGET) data/scene_cube2.txt 800 800

cube3: $(TARGET)
	./$(TARGET) data/scene_cube3.txt 800 800

bunny1: $(TARGET)
	./$(TARGET) data/scene_bunny1.txt 800 800

bunny1_attenuated: $(TARGET)
	./$(TARGET) data/scene_bunny1_attenuated.txt 800 800

bunny2: $(TARGET)
	./$(TARGET) data/scene_bunny2.txt 800 800

fourCubes: $(TARGET)
	./$(TARGET) data/scene_fourCubes.txt 800 800

kitten: $(TARGET)
	./$(TARGET) data/scene_kitten.txt 800 800

kitten_attenuated: $(TARGET)
	./$(TARGET) data/scene_kitten_attenuated.txt 800 800

armadillo: $(TARGET)
	./$(TARGET) data/scene_armadillo.txt 800 800

sphere: $(TARGET)
	./$(TARGET) data/scene_sphere.txt 800 800

sphere_attenuated: $(TARGET)
	./$(TARGET) data/scene_sphere_attenuated.txt 800 800

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIBS)"; $(CC) $^ -o $(TARGET) $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) $(LIBDIR) $(LIBS) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) $(LIBDIR) $(LIBS) -c -o $@ $<

clean:
	@echo " Cleaning..."
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# Tests
test: tester
	./bin/tester

tester: $(OBJECTS)
	$(CC) $(CFLAGS) $(INC) -lboost_unit_test_framework test/tester.cpp -o bin/tester

.PHONY: all clean
