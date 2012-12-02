OPENCL_HOME ?= /opt/AMDAPP
OPENCL_INC ?= -I $(OPENCL_HOME)/include
OPENCL_LIBS ?= -L $(OPENCL_HOME)/lib/x86_64
CUDA_HOME ?= /usr/local/cuda
#CUDA_INC ?= -I $(CUDA_HOME)/include
#CUDA_LIBS ?= -L $(CUDA_HOME)/lib64

CPP := g++
LINK := g++
CLCC := clcc

