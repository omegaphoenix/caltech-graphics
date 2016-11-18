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
TARGET = bin/opengl_renderer

# The following line is a relative directory reference that assumes the Eigen
# folder--which your program will depend on--is located one directory above the
# directory that contains this Makefile.
SRCEXT := cpp
SOURCES:=$(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
INC := -I include -I/usr/X11R6/include -I/usr/include/GL -I/usr/include
LIBS = -lGLEW -lGL -lGLU -lglut -lm
LIBDIR = -L/usr/X11R6/lib -L/usr/local/lib

bunny: $(TARGET)
	./$(TARGET) data/scene_bunny.txt 800 800 0.01

armadillo: $(TARGET)
	./$(TARGET) data/scene_armadillo.txt 800 800 0.01

all: $(TARGET) bunny armadillo

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
