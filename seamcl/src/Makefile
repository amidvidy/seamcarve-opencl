OPENCL_HOME=/opt/AMDAPP
OPENCL_INC=$(OPENCL_HOME)/include
OPENCL_LIB=$(OPENCL_HOME)/lib/$(shell uname -p)

BUILDDIR = build

CLSRC = $(wildcard *.cl)
CLOUT = $(CLSRC:%.cl=$(BUILDDIR)/%.out)

OBJECTS = seamcl.cpp

all: seamcl

clout: $(CLOUT)

seamcl: $(OBJECTS)
	g++ $< -Wall -g -I $(OPENCL_INC) -L $(OPENCL_LIB) -lOpenCL -lfreeimage -o $@

seamcl.o : image.hpp kernel.hpp math.hpp mem.hpp setup.hpp
	g++ -c -g seamcl.cpp

$(BUILDDIR)/%.out: %.cl | $(BUILDDIR)
	clcc --add_headers -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR) seamcl

.PHONY: all clean fresh
