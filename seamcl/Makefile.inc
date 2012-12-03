OPENCL_HOME ?= /opt/AMDAPP
#OPENCL_INCD ?= -I $(OPENCL_HOME)/include
#OPENCL_LIBD ?= -L $(OPENCL_HOME)/lib/x86_64
#OPENCL_LIBD ?= -framework OpenCL

CUDA_HOME ?= /usr/local/cuda
#CUDA_INCD ?= -I $(CUDA_HOME)/include
#CUDA_LIBD ?= -L $(CUDA_HOME)/lib64

FREEIMG_INCD ?= -I FreeImage/include
FREEIMG_LIBD ?= -L FreeImage/lib/x64

CPP := g++
LINK := g++
CLCC := clcc

INC_DIRS := $(CUDA_INCD) $(OPENCL_INCD) $(FREEIMG_INCD)
LIB_DIRS := $(CUDA_LIBD) $(OPENCL_LIBD) $(FREEIMG_LIBD)
LIBS ?= -lOpenCL -lfreeimage

