include ../Makefile.inc

# Expects each program to be a single cpp file
CPs := $(wildcard *.cpp)
CLs := $(wildcard *.cl)
CPo := $(CPs:%.cpp=%)
CLo := $(CLs:%.cl=%.out)
TARGETS ?= $(CPo) $(CLo)
#TARGETS ?= OpenCLInfo probe ImageFilter2D ImageFilter2D.out

all: $(TARGETS)

%: %.cpp
	@echo -e "\n\n$<\n"
	-g++ $< -Wall -O0 -g -I ../include $(CUDA_LIBS) $(OPENCL_LIBS) -o $@

%.out: %.cl
	@echo -e "\n\n$<\n"
	-clcc --add_headers $< -o $@

clean:
	rm -rf *.dSYM $(TARGETS)

.PHONY: all clean
