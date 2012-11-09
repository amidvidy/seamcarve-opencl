APPNAME ?= seamc
BUILDIR ?= build

CSRCS := $(wildcard *.c)
PSRCS := $(wildcard *.cpp)

COBJS := $(CSRCS:%.c=$(BUILDIR)/%.c.o)
POBJS := $(PSRCS:%.cpp=$(BUILDIR)/%.cpp.o)

OBJ = $(COBJS) $(POBJS)

WandINCS ?= `pkg-config --cflags MagickWand` #`MagickWand-config --cflags --cppflags`
WandLIBS ?= `pkg-config --libs MagickWand` #`MagickWand-config --ldflags --libs`

INCS += $(WandINCS)
LIBS += $(WandLIBS) -pthread -fopenmp

CC ?= gcc
CP ?= g++


default: all

all: $(APPNAME)

$(APPNAME): $(BUILDIR) | $(OBJ)
	gcc $(OBJ) $(LIBS) -o $@

$(BUILDIR)/%.c.o: %.c
	$(CC) -O3 $(INCS) -c $< -o $@

$(BUILDIR)/%.cpp.o: %.cpp
	$(CP) -O3 $(INCS) -c $< -o $@

proto:
	cat $(CSRCS) | grep -e "^[[:alnum:]_]*[[:space:]]*[[:alnum:]_]*[[:space:]]*([^;]*)[[:space:]]*{$$" | sed -e "s/[[:space:]]*{$$/;/"

$(BUILDIR):
	mkdir -p $(BUILDIR)

clean:
	rm -rf build $(APPNAME)

.PHONY: all clean

