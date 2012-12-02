OPENCL_HOME ?= /opt/AMDAPP/include

THINGS := OpenCLInfo probe ImageFilter2D ImageFilter2D.out

all: $(THINGS)

%: %.cpp
	g++ $< -I $(OPENCL_HOME) -lOpenCL -lfreeimage -o $@

%.out: %.cl
	clcc --add_headers $< -o $@

clean:
	rm -rf $(THINGS)

.PHONY: all clean
