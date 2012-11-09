APPNAME ?= seamc
BUILDIR ?= build
TESTIMG ?= img.jpg
FLAGS ?= -std=c99 -O3
LIBS ?= -pthread -fopenmp

CSRCS := $(wildcard *.c)
HDR   := $(wildcard *.h *.hpp)

OBJ := $(CSRCS:%.c=$(BUILDIR)/%.c.o)

WandFLAGS ?= `pkg-config --cflags MagickWand` #`MagickWand-config --cflags --cppflags`
WandLIBS  ?= `pkg-config --libs MagickWand` #`MagickWand-config --ldflags --libs`

FLAGS += $(WandFLAGS)
LIBS  += $(WandLIBS)

CC ?= gcc # I guess is ok to do this rather than use builtin Make stuff!
CP ?= g++


default: test

all: $(APPNAME)

fresh: clean all

test: all
	./$(APPNAME) $(TESTIMG)

$(APPNAME): $(BUILDIR)/headers $(BUILDIR) $(OBJ)
	gcc $(OBJ) $(LIBS) -o $@

$(BUILDIR)/headers: $(HDR) | $(BUILDIR)
	rm -f $(OBJ) #Header changed, remove ALL object code!
	@touch $@ #Track single mod date of all header files

$(BUILDIR)/%.c.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

proto:
	@cat $(CSRCS) | grep -e "^[[:alnum:]_]*[[:space:]]*[[:alnum:]_]*[[:space:]]*([^;]*)[[:space:]]*{$$" | sed -e "s/[[:space:]]*{$$/;/"

$(BUILDIR):
	mkdir -p $(BUILDIR)

clean:
	rm -rf build $(APPNAME)

.PHONY: all clean fresh test proto

