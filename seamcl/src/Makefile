OPENCL_HOME=/opt/AMDAPP/include

OBJECTS = seamcl.cpp

all: seamcl

seamcl: $(OBJECTS)
	g++ $< -Wall -I $(OPENCL_HOME) -lOpenCL -lfreeimage -o $@

seamcl.o : image.hpp kernel.hpp math.hpp mem.hpp setup.hpp
	g++ -c seamcl.cpp

clean:
	rm -rf seamcl

.PHONY: all clean fresh
