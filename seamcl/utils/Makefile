OPENCL_HOME=/opt/AMDAPP/include

THING=OpenCLInfo

all: $(THING)

$(THING): $(THING).cpp
	g++ $< -I $(OPENCL_HOME) -lOpenCL -lfreeimage -o $@

clean:
	rm -rf $(THING)

.PHONY: all clean fresh
