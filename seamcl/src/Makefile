OPENCL_HOME=/opt/AMDAPP/include

BUILDDIR = build

CLSRC = $(wildcard *.cl)
CLOUT = $(CLSRC:%.cl=$(BUILDDIR)/%.out)

OBJECTS = seamcl.cpp

all: seamcl

clout: $(CLOUT)

seamcl: $(OBJECTS)
	g++ $< -Wall -g -I $(OPENCL_HOME) -lOpenCL -lfreeimage -o $@

seamcl.o : image.hpp kernel.hpp math.hpp mem.hpp setup.hpp
	g++ -c -g seamcl.cpp

$(BUILDDIR)/%.out: %.cl | $(BUILDDIR)
	clcc -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf seamcl

.PHONY: all clean fresh
