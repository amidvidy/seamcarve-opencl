OPENCL_HOME=/opt/AMDAPP/include

all: seamcl

seamcl: seamcl.cpp
	g++ $< -I $(OPENCL_HOME) -lOpenCL -lfreeimage -o $@

clean:
	rm -rf seamcl

.PHONY: all clean fresh
