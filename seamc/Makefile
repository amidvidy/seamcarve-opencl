APPNAME ?= seamc

CSRCS := $(wildcard *.c)
PSRCS := $(wildcard *.cpp)
COBJS := $(CSRCS:%.c=%.c.o)
POBJS := $(PSRCS:%.cpp=%.cpp.o)

OBJ = $(COBJS) $(POBJS)

WandINCS ?= `pkg-config --cflags MagickWand` #`MagickWand-config --cflags --cppflags`
WandLIBS ?= `pkg-config --libs MagickWand` #`MagickWand-config --ldflags --libs`

INCS += $(WandINCS)
LIBS += $(WandLIBS) -pthread -fopenmp

CC ?= gcc
CP ?= g++

default: all

all: $(OBJ)
	gcc $(OBJ) $(LIBS) -o $(APPNAME)

%.c.o: %.c
	$(CC) -O3 $(INCS) -c $< -o $@

%.cpp.o: %.cpp
	$(CP) -O3 $(INCS) -c $< -o $@

clean:
	rm -rf $(APPNAME) $(COBJS) $(POBJS)

.PHONY: all clean

