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
TARGET = bin/shaded

# The following line is a relative directory reference that assumes the Eigen
# folder--which your program will depend on--is located one directory above the
# directory that contains this Makefile.
SRCEXT := cpp
SOURCES:=$(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
INC := -I include

all: $(TARGET)
	./$(TARGET) data/scene_cube1.txt 800 800 > data/scene_cube1.new
	diff data/scene_cube1.new data/scene_cube1.expected
	./$(TARGET) data/scene_cube2.txt 800 800 > data/scene_cube2.new
	diff data/scene_cube2.new data/scene_cube2.expected
	./$(TARGET) data/scene_face1.txt 800 800 > data/scene_face1.new
	diff data/scene_face1.new data/scene_face1.expected
	./$(TARGET) data/scene_face2.txt 800 800 > data/scene_face2.new
	diff data/scene_face2.new data/scene_face2.expected
	./$(TARGET) data/scene_fourCubes.txt 800 800 > data/scene_fourCubes.new
	diff data/scene_fourCubes.new data/scene_fourCubes.expected
	./$(TARGET) data/scene_bunny1.txt 800 800 > data/scene_bunny1.new
	diff data/scene_bunny1.new data/scene_bunny1.expected
	./$(TARGET) data/scene_bunny2.txt 800 800 > data/scene_bunny2.new
	diff data/scene_bunny2.new data/scene_bunny2.expected
	./$(TARGET) data/scene_bunny_closeup.txt 800 800 > data/scene_bunny_closeup.new
	diff data/scene_bunny_closeup.new data/scene_bunny_closeup.expected

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET)"; $(CC) $^ -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# Tests
test: tester
	./bin/tester

tester: $(OBJECTS)
	$(CC) $(CFLAGS) $(INC) -lboost_unit_test_framework test/tester.cpp -o bin/tester

.PHONY: all clean
