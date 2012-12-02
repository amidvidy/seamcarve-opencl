OPENCL_HOME ?= /opt/AMDAPP/include

TARGET ?= seamcl
BUILDDIR ?= build
SRCDIR ?= src
KERNDIR ?= src
INCDIR ?= include
dependfile ?= Makefile.dep

CPP := g++
LINK := g++
CLCC := clcc

CLSRC := $(wildcard $(KERNDIR)/*.cl)
CLOUT := $(CLSRC:$(KERNDIR)/%.cl=$(BUILDDIR)/%.out)
CLOPT += --add_headers

CSRC := $(wildcard $(SRCDIR)/*.cpp)
CHDR := $(wildcard $(INCDIR)/*.h $(INCDIR)/*.hpp)
OBJECTS := $(CSRC:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
CLIB += -lOpenCL -lfreeimage
CINC += -I $(INCDIR) -I $(OPENCL_HOME)
CFLAG_DBG ?= -g3 -O0
CFLAG += $(CFLAG_DBG) -Wall --std=c++0x


default: $(TARGET)

settings:
	@echo $(CPP) $(CFLAG) $(CINC) $(CLIB)
	@echo $(CSRC) $(CHDR)
	@echo $(OBJECTS) $(TARGET)

all: $(TARGET) $(CLOUT) utils

$(TARGET): $(OBJECTS)
	$(LINK) $(OBJECTS) $(CLIB) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CPP) -o $@ -c $< $(CFLAG) $(CINC)

$(BUILDDIR)/%.out: $(KERNDIR)/%.cl | $(BUILDDIR)
	-$(CLCC) $(CLOPT) -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)


.PHONY: all clean dep clout utils

clout: $(CLOUT)

clean:
	rm -rf $(BUILDDIR) $(dependfile) $(TARGET)
	cd utils && $(MAKE) $(MAKEFLAGS) clean

utils:
	cd utils && $(MAKE) $(MAKEFLAGS)

$(dependfile): $(CSRC) $(CHDR) | $(BUILDDIR)
	$(CPP) -MM $(CFLAG) $(CSRC) $(CINC) > $@

dep: $(dependfile)


ifneq ($(MAKECMDGOALS),dep)
 ifneq ($(MAKECMDGOALS),clean)
  include $(dependfile)
 endif
endif

