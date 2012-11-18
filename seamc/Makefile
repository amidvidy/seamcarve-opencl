APPNAME ?= seamc
BUILDIR ?= build
REFDIR ?= ../ref_images/seam_img
TESTIMG ?= $(REFDIR)/in.tif
REFIMG ?= $(REFDIR)/out.tif
FLAGS ?= -O0 -Wall -g -fno-inline-functions
LIBS ?= -pthread -fopenmp

CSRCS := $(wildcard *.c)
HDR   := $(wildcard *.h *.hpp)

OBJ := $(CSRCS:%.c=$(BUILDIR)/%.c.o)

WandFLAGS ?= `pkg-config --cflags MagickWand` #`MagickWand-config --cflags --cppflags`
WandLIBS  ?= `pkg-config --libs MagickWand` #`MagickWand-config --ldflags --libs`

FLAGS += -std=c99 $(WandFLAGS)
LIBS  += $(WandLIBS)

CC ?= gcc # I guess is ok to do this rather than use builtin Make stuff!
CP ?= g++


default: test

all: $(APPNAME)

fresh: clean all

test: all
	rm -f out.tif
	identify $(TESTIMG)
	./$(APPNAME) $(TESTIMG)
	identify out.tif

view: test
	display out.tif

diff: test
	identify $(REFIMG)
	compare $(REFIMG) out.tif diff.tif
	display diff.tif out.tif

$(APPNAME): $(BUILDIR)/headers $(OBJ) | $(BUILDIR)
	gcc $(OBJ) $(LIBS) -o $@

$(BUILDIR)/headers: $(HDR) | $(BUILDIR)
	rm -f $(OBJ) #Header changed, remove ALL object code!
	@touch $@ #Track single mod date of all header files

$(BUILDIR)/%.c.o: %.c | $(BUILDIR)/headers
	$(CC) $(FLAGS) -c $< -o $@

obj: $(OBJ)

proto:
	@cat $(CSRCS) | grep -e "^[[:alnum:]_]*[[:space:]]*[[:alnum:]_]*[[:space:]]*([^;]*)[[:space:]]*{$$" | sed -e "s/[[:space:]]*{$$/;/"

$(BUILDIR):
	mkdir -p $(BUILDIR)

clean:
	rm -rf build $(APPNAME) out.* diff.*

.PHONY: clean fresh test proto obj

