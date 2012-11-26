OPENCL_HOME=/opt/AMDAPP/include

all: ImageFilter2D

ImageFilter2D: ImageFilter2D.cpp
	g++ $< -I $(OPENCL_HOME) -lOpenCL -lfreeimage -o $@

clean:
	rm -rf ImageFilter2D

.PHONY: all clean fresh
