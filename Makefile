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
	./$(TARGET) data/scene_cube2.txt 800 800 1 | display

# Simple benchmark
benchmark: $(TARGET)
	time ./$(TARGET) data/scene_armadillo.txt 800 800 0 > armadillo.temp
	time ./$(TARGET) data/scene_armadillo.txt 800 800 1 > armadillo.temp
	rm armadillo.temp

# Quick testing for refactoring
fast: $(TARGET)
	./$(TARGET) data/scene_fourCubes.txt 100 100 0 > fourCubes_Gouraud.new
	diff fourCubes_Gouraud.new fourCubes_Gouraud.expected
	./$(TARGET) data/scene_fourCubes.txt 100 100 0 > fourCubes_Gouraud.new
	diff fourCubes_Gouraud.new fourCubes_Gouraud.expected

# Exhaustive testing for refactoring
check: $(TARGET)
	./$(TARGET) data/scene_cube1.txt 100 100 0 > cube1_Gouraud.new
	diff cube1_Gouraud.new cube1_Gouraud.expected
	./$(TARGET) data/scene_cube2.txt 100 100 0 > cube2_Gouraud.new
	diff cube2_Gouraud.new cube2_Gouraud.expected
	./$(TARGET) data/scene_cube3.txt 100 100 0 > cube3_Gouraud.new
	diff cube3_Gouraud.new cube3_Gouraud.expected
	./$(TARGET) data/scene_bunny1.txt 100 100 0 > bunny1_Gouraud.new
	diff bunny1_Gouraud.new bunny1_Gouraud.expected
	./$(TARGET) data/scene_bunny1_attenuated.txt 100 100 0 > bunny1_attenuated_Gouraud.new
	diff bunny1_attenuated_Gouraud.new bunny1_attenuated_Gouraud.expected
	./$(TARGET) data/scene_bunny2.txt 100 100 0 > bunny2_Gouraud.new
	diff bunny2_Gouraud.new bunny2_Gouraud.expected
	./$(TARGET) data/scene_fourCubes.txt 100 100 0 > fourCubes_Gouraud.new
	diff fourCubes_Gouraud.new fourCubes_Gouraud.expected
	./$(TARGET) data/scene_kitten.txt 100 100 0 > kitten_Gouraud.new
	diff kitten_Gouraud.new kitten_Gouraud.expected
	./$(TARGET) data/scene_kitten_attenuated.txt 100 100 0 > kitten_attenuated_Gouraud.new
	diff kitten_attenuated_Gouraud.new kitten_attenuated_Gouraud.expected
	./$(TARGET) data/scene_armadillo.txt 100 100 0 > armadillo_Gouraud.new
	diff armadillo_Gouraud.new armadillo_Gouraud.expected
	./$(TARGET) data/scene_sphere.txt 100 100 0 > sphere_Gouraud.new
	diff sphere_Gouraud.new sphere_Gouraud.expected
	./$(TARGET) data/scene_sphere_attenuated.txt 100 100 0 > sphere_attenuated_Gouraud.new
	diff sphere_attenuated_Gouraud.new sphere_attenuated_Gouraud.expected
	./$(TARGET) data/scene_cube1.txt 100 100 1 > cube1_Phong.new
	diff cube1_Phong.new cube1_Phong.expected
	./$(TARGET) data/scene_cube2.txt 100 100 1 > cube2_Phong.new
	diff cube2_Phong.new cube2_Phong.expected
	./$(TARGET) data/scene_cube3.txt 100 100 1 > cube3_Phong.new
	diff cube3_Phong.new cube3_Phong.expected
	./$(TARGET) data/scene_bunny1.txt 100 100 1 > bunny1_Phong.new
	diff bunny1_Phong.new bunny1_Phong.expected
	./$(TARGET) data/scene_bunny1_attenuated.txt 100 100 1 > bunny1_attenuated_Phong.new
	diff bunny1_attenuated_Phong.new bunny1_attenuated_Phong.expected
	./$(TARGET) data/scene_bunny2.txt 100 100 1 > bunny2_Phong.new
	diff bunny2_Phong.new bunny2_Phong.expected
	./$(TARGET) data/scene_fourCubes.txt 100 100 1 > fourCubes_Phong.new
	diff fourCubes_Phong.new fourCubes_Phong.expected
	./$(TARGET) data/scene_kitten.txt 100 100 1 > kitten_Phong.new
	diff kitten_Phong.new kitten_Phong.expected
	./$(TARGET) data/scene_kitten_attenuated.txt 100 100 1 > kitten_attenuated_Phong.new
	diff kitten_attenuated_Phong.new kitten_attenuated_Phong.expected
	./$(TARGET) data/scene_armadillo.txt 100 100 1 > armadillo_Phong.new
	diff armadillo_Phong.new armadillo_Phong.expected
	./$(TARGET) data/scene_sphere.txt 100 100 1 > sphere_Phong.new
	diff sphere_Phong.new sphere_Phong.expected
	./$(TARGET) data/scene_sphere_attenuated.txt 100 100 1 > sphere_attenuated_Phong.new
	diff sphere_attenuated_Phong.new sphere_attenuated_Phong.expected

# Create files for exhaustive testing for refactoring
update_expected: $(TARGET)
	./$(TARGET) data/scene_cube1.txt 100 100 0 > cube1_Gouraud.expected
	./$(TARGET) data/scene_cube2.txt 100 100 0 > cube2_Gouraud.expected
	./$(TARGET) data/scene_cube3.txt 100 100 0 > cube3_Gouraud.expected
	./$(TARGET) data/scene_bunny1.txt 100 100 0 > bunny1_Gouraud.expected
	./$(TARGET) data/scene_bunny1_attenuated.txt 100 100 0 > bunny1_attenuated_Gouraud.expected
	./$(TARGET) data/scene_bunny2.txt 100 100 0 > bunny2_Gouraud.expected
	./$(TARGET) data/scene_fourCubes.txt 100 100 0 > fourCubes_Gouraud.expected
	./$(TARGET) data/scene_kitten.txt 100 100 0 > kitten_Gouraud.expected
	./$(TARGET) data/scene_kitten_attenuated.txt 100 100 0 > kitten_attenuated_Gouraud.expected
	./$(TARGET) data/scene_armadillo.txt 100 100 0 > armadillo_Gouraud.expected
	./$(TARGET) data/scene_sphere.txt 100 100 0 > sphere_Gouraud.expected
	./$(TARGET) data/scene_sphere_attenuated.txt 100 100 0 > sphere_attenuated_Gouraud.expected
	./$(TARGET) data/scene_cube1.txt 100 100 1 > cube1_Phong.expected
	./$(TARGET) data/scene_cube2.txt 100 100 1 > cube2_Phong.expected
	./$(TARGET) data/scene_cube3.txt 100 100 1 > cube3_Phong.expected
	./$(TARGET) data/scene_bunny1.txt 100 100 1 > bunny1_Phong.expected
	./$(TARGET) data/scene_bunny1_attenuated.txt 100 100 1 > bunny1_attenuated_Phong.expected
	./$(TARGET) data/scene_bunny2.txt 100 100 1 > bunny2_Phong.expected
	./$(TARGET) data/scene_fourCubes.txt 100 100 1 > fourCubes_Phong.expected
	./$(TARGET) data/scene_kitten.txt 100 100 1 > kitten_Phong.expected
	./$(TARGET) data/scene_kitten_attenuated.txt 100 100 1 > kitten_attenuated_Phong.expected
	./$(TARGET) data/scene_armadillo.txt 100 100 1 > armadillo_Phong.expected
	./$(TARGET) data/scene_sphere.txt 100 100 1 > sphere_Phong.expected
	./$(TARGET) data/scene_sphere_attenuated.txt 100 100 1 > sphere_attenuated_Phong.expected

cube1: $(TARGET)
	./$(TARGET) data/scene_cube1.txt 800 800 0 | display
	./$(TARGET) data/scene_cube1.txt 800 800 1 | display

cube2: $(TARGET)
	./$(TARGET) data/scene_cube2.txt 800 800 0 | display
	./$(TARGET) data/scene_cube2.txt 800 800 1 | display

cube3: $(TARGET)
	./$(TARGET) data/scene_cube3.txt 800 800 0 | display
	./$(TARGET) data/scene_cube3.txt 800 800 1 | display

bunny1: $(TARGET)
	./$(TARGET) data/scene_bunny1.txt 800 800 0 | display
	./$(TARGET) data/scene_bunny1.txt 800 800 1 | display

bunny1_attenuated: $(TARGET)
	./$(TARGET) data/scene_bunny1_attenuated.txt 800 800 0 | display
	./$(TARGET) data/scene_bunny1_attenuated.txt 800 800 1 | display

bunny2: $(TARGET)
	./$(TARGET) data/scene_bunny2.txt 800 800 0 | display
	./$(TARGET) data/scene_bunny2.txt 800 800 1 | display

fourCubes: $(TARGET)
	./$(TARGET) data/scene_fourCubes.txt 800 800 0 | display
	./$(TARGET) data/scene_fourCubes.txt 800 800 1 | display

kitten: $(TARGET)
	./$(TARGET) data/scene_kitten.txt 800 800 0 | display
	./$(TARGET) data/scene_kitten.txt 800 800 1 | display

kitten_attenuated: $(TARGET)
	./$(TARGET) data/scene_kitten_attenuated.txt 800 800 0 | display
	./$(TARGET) data/scene_kitten_attenuated.txt 800 800 1 | display

armadillo: $(TARGET)
	./$(TARGET) data/scene_armadillo.txt 800 800 0 | display
	./$(TARGET) data/scene_armadillo.txt 800 800 1 | display

sphere: $(TARGET)
	./$(TARGET) data/scene_sphere.txt 800 800 0 | display
	./$(TARGET) data/scene_sphere.txt 800 800 1 | display

sphere_attenuated: $(TARGET)
	./$(TARGET) data/scene_sphere_attenuated.txt 800 800 0 | display
	./$(TARGET) data/scene_sphere_attenuated.txt 800 800 1 | display

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET)"; $(CC) $^ -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."
	@echo " $(RM) -r $(BUILDDIR) $(TARGET) *.expected *.new"; $(RM) -r $(BUILDDIR) $(TARGET) *.expected *.new

# Tests
test: tester
	./bin/tester

tester: $(OBJECTS)
	$(CC) $(CFLAGS) $(INC) -lboost_unit_test_framework test/tester.cpp -o bin/tester

.PHONY: all clean
