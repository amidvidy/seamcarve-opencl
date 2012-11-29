OPENCL_HOME=/opt/AMDAPP/include

BUILDDIR = build

CLSRC = $(wildcard *.cl)
CLOUT = $(CLSRC:%.cl=$(BUILDDIR)/%.out)

OBJECTS = seamcl.cpp

all: seamcl

clout: $(CLOUT)

seamcl: $(OBJECTS)
	g++ $< -Wall -O2 -I $(OPENCL_HOME) -lOpenCL -lfreeimage -o $@

seamcl.o : image.hpp kernel.hpp math.hpp mem.hpp setup.hpp
	g++ -c -O2 seamcl.cpp

$(BUILDDIR)/%.out: %.cl | $(BUILDDIR)
	clcc --add_headers -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR) seamcl

.PHONY: all clean fresh
