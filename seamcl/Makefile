OPENCL_HOME ?= /opt/AMDAPP/include

TARGET ?= seamcl
BUILDDIR ?= build
SRCDIR ?= src
KERNDIR ?= src
INCDIR ?= include

CPP := g++
LINK := g++
CLCC := clcc

CLSRC := $(wildcard $(KERNDIR)/*.cl)
CLOUT := $(CLSRC:$(KERNDIR)/%.cl=$(BUILDDIR)/%.out)
CLOPT += --add_headers

CSRC := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(CSRC:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
CLIB += -lOpenCL -lfreeimage
CINC += -I $(INCDIR) -I $(OPENCL_HOME)
CFLAG += -O0 -Wall --std=c++0x


default: $(TARGET)

settings:
	echo $(CLIB)
	echo $(CSRC)

all: $(TARGET) $(CLOUT)

$(TARGET): $(OBJECTS)
	$(LINK) $(OBJECTS) $(CLIB) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CPP) -o $@ -c $< $(CFLAG) $(CINC)

$(BUILDDIR)/%.out: $(KERNDIR)/%.cl | $(BUILDDIR)
	-$(CLCC) $(CLOPT) -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)


.PHONY: all clean fresh clout

clout: $(CLOUT)

clean:
	rm -rf $(BUILDDIR) seamcl

fresh: clean all

