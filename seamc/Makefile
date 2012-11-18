REFDIR ?= ../ref_images/castle-9
IMGEXT ?= tif
TESTIMG ?= $(REFDIR)/in.$(IMGEXT)
REFIMG ?= $(REFDIR)/out.$(IMGEXT)
OUTIMG ?= out.$(IMGEXT)
DIFFIMG ?= diff.$(IMGEXT)

APPNAME ?= seamc
BUILDIR ?= build

FLAGS ?= -O0 -Wall -g -fno-inline-functions
LIBS ?= -pthread -fopenmp

CSRCS := $(wildcard *.c)
CPSRCS:= $(wildcard *.cpp)
HDR   := $(wildcard *.h *.hpp)

OBJ := $(CSRCS:%.c=$(BUILDIR)/%.c.o) $(CPSRCS:%.cpp=$(BUILDIR)/%.cpp.o)

WandFLAGS ?= `pkg-config --cflags MagickWand` #`MagickWand-config --cflags --cppflags`
WandLIBS  ?= `pkg-config --libs MagickWand` #`MagickWand-config --ldflags --libs`

CFLAGS = -std=c99 $(WandFLAGS) $(FLAGS)
CPFLAGS = $(WandFLAGS) $(FLAGS)
LIBS  += $(WandLIBS)

CC ?= gcc
CP ?= g++


default: test

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
	gcc $(OBJ) $(LIBS) -o $@

$(BUILDIR)/headers: $(HDR) | $(BUILDIR)
	#rm -f $(OBJ) #Header changed, remove ALL object code!
	@touch $@ #Track single mod date of all header files

$(BUILDIR)/%.c.o: %.c | $(BUILDIR)/headers
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDIR)/%.cpp.o: %.cpp | $(BUILDIR)/headers
	$(CP) $(CPFLAGS) -c $< -o $@

obj: $(OBJ)

settings:


$(BUILDIR):
	mkdir -p $(BUILDIR)

clean:
	rm -rf build $(APPNAME) out.* diff.*

.PHONY: clean fresh obj test view diff settings

