REFDIR ?= ../ref_images/castle.jpg_py10
IMGEXT ?= tif
TESTIMG ?= $(REFDIR)/in.$(IMGEXT)
REFIMG ?= $(REFDIR)/out.$(IMGEXT)
OUTIMG ?= out.$(IMGEXT)
DIFFIMG ?= diff.$(IMGEXT)

CUDAHOME ?= /usr/local/cuda

APPNAME ?= seamc
BUILDIR ?= build

#FLAGS ?= -Wall -O3
FLAGS ?= -Wall -O0 -g -fno-inline-functions -I include -I $(CUDAHOME)/include
LIBS ?= -pthread -fopenmp -lfreeimage -lnpp -lcudart -L $(CUDAHOME)/lib64

CSRCS := $(wildcard src/*.c)
CPSRCS:= $(wildcard src/*.cpp)
HDR   := $(wildcard include/*.h)

OBJ := $(CSRCS:src/%.c=$(BUILDIR)/%.o) $(CPSRCS:src/%.cpp=$(BUILDIR)/%.o)

WandFLAGS ?= `pkg-config --cflags MagickWand` #`MagickWand-config --cflags --cppflags`
WandLIBS  ?= `pkg-config --libs MagickWand` #`MagickWand-config --ldflags --libs`

CFLAGS = -std=c99 $(WandFLAGS) $(FLAGS)
CPFLAGS = $(WandFLAGS) $(FLAGS)
LIBS  += $(WandLIBS)

CC ?= gcc
CP ?= g++


default: all

all: $(APPNAME)

fresh: clean all

test: $(OUTIMG)

$(OUTIMG): $(APPNAME)
	-rm -f $@
	identify $(TESTIMG)
	./$(APPNAME) $(TESTIMG)
	identify $@

view: $(OUTIMG)
	display $<

diff: $(OUTIMG)
	-rm -f $(DIFFIMG)
	identify $(REFIMG)
	compare $(REFIMG) $(OUTIMG) $(DIFFIMG)
	display $(DIFFIMG)

$(APPNAME): $(BUILDIR)/headers $(OBJ) | $(BUILDIR)
	g++ $(OBJ) $(LIBS) -o $@

$(BUILDIR)/headers: $(HDR) | $(BUILDIR)
	#rm -f $(OBJ) #Header changed, remove ALL object code!
	@touch $@ #Track single mod date of all header files

$(BUILDIR)/%.o: src/%.c | $(BUILDIR)/headers
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDIR)/%.o: src/%.cpp | $(BUILDIR)/headers
	$(CP) $(CPFLAGS) -c $< -o $@


settings:


$(BUILDIR):
	mkdir -p $(BUILDIR)

clean:
	rm -rf build $(APPNAME) out.* diff.*

.PHONY: clean fresh test view diff settings $(OUTIMG)

