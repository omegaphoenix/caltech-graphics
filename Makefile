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
	./$(TARGET) data/scene_cube1.txt 800 800 0

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
