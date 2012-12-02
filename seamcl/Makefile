OPENCL_HOME ?= /opt/AMDAPP

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
LIBS += -lOpenCL -lfreeimage
LIBDIRS += -L $(OPENCL_HOME)/lib/x86_64
INCDIRS += -I $(INCDIR) -I $(OPENCL_HOME)/include
CFLAG_DBG ?= -g3 -O0
CFLAG += $(CFLAG_DBG) -Wall --std=c++0x


default: $(TARGET)

settings:
	@echo $(CPP) $(CFLAG) $(INCDIR) $(LIBDIRS) $(LIBS)
	@echo $(CSRC) $(CHDR)
	@echo $(OBJECTS) $(TARGET)

all: $(TARGET) $(CLOUT) utils

$(TARGET): $(OBJECTS)
	$(LINK) $(OBJECTS) $(LIBDIRS) $(LIBS) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CPP) -o $@ -c $< $(CFLAG) $(INCDIRS)

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
	$(CPP) -MM $(CFLAG) $(CSRC) $(INCDIRS) > $@

dep: $(dependfile)


ifneq ($(MAKECMDGOALS),dep)
 ifneq ($(MAKECMDGOALS),clean)
  include $(dependfile)
 endif
endif

