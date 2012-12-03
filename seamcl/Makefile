include Makefile.inc

TARGETS ?= seamcl
BUILDDIR ?= build
SRCDIR ?= src
KERNDIR ?= src
INCDIR ?= include
dependfile ?= Makefile.dep

CLSRC := $(wildcard $(KERNDIR)/*.cl)
CLOUT := $(CLSRC:$(KERNDIR)/%.cl=$(BUILDDIR)/%.out)
CLOPT += --add_headers

CSRC := $(wildcard $(SRCDIR)/*.cpp)
CHDR := $(wildcard $(INCDIR)/*.h $(INCDIR)/*.hpp)
OBJECTS := $(CSRC:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
INC_DIRS += -I $(INCDIR)
CFLAG_DBG ?= -g3 -O0
CFLAG += $(CFLAG_DBG) -Wall


default: $(TARGETS)

settings:
	@echo $(CPP) $(CFLAG) $(INC_DIRS) $(LIB_DIRS) $(LIBS)
	@echo $(CSRC) $(CHDR)
	@echo $(OBJECTS) $(TARGETS)

all: $(TARGETS) $(CLOUT) utils

$(TARGETS): $(OBJECTS)
	$(LINK) $(OBJECTS) $(LIB_DIRS) $(LIBS) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CPP) -o $@ -c $< $(CFLAG) $(INC_DIRS)

$(BUILDDIR)/%.out: $(KERNDIR)/%.cl | $(BUILDDIR)
	-$(CLCC) $(CLOPT) -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)


.PHONY: all clean dep clout utils

clout: $(CLOUT)

clean:
	rm -rf $(BUILDDIR) $(dependfile) $(TARGETS)
	cd utils && $(MAKE) $(MAKEFLAGS) clean

utils:
	cd utils && $(MAKE) $(MAKEFLAGS)

$(dependfile): $(CSRC) $(CHDR) | $(BUILDDIR)
	$(CPP) -MM $(CFLAG) $(CSRC) $(INC_DIRS) > $@

dep: $(dependfile)


ifneq ($(MAKECMDGOALS),dep)
 ifneq ($(MAKECMDGOALS),clean)
  include $(dependfile)
 endif
endif

