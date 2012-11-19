#Requires ImageMagick on the path (convert)

SUFX ?= tif png gif jpg
CARV ?= 1 10

ORIG := $(wildcard $(foreach sufx,$(SUFX),orig/*.$(sufx)))
CONV := $(ORIG:orig/%=ppm/%.ppm)

IMGS = $(sort $(wildcard ppm/*.ppm) $(CONV))
DIRS = $(foreach carv,$(CARV),$(IMGS:ppm/%.ppm=%_py$(carv)))


default: info

all: conv $(DIRS)

fresh: clean all

conv: $(CONV)

dirs:
	echo $(IMGS)
	echo $(DIRS)

ppm/%.ppm: orig/% | ppm
	convert -strip $< -colorspace RGB -depth 8 $@

%_py1: ppm/%.ppm | carve.py
	rm -Rf $@
	( mkdir -p $@ && cd $@ && ../carve.py %< 1 )

%_py10: ppm/%.ppm | carve.py
	rm -Rf $@
	( mkdir -p $@ && cd $@ && ../carve.py $< 1 )

ppm:
	mkdir -p ppm

.PHONY: clean info

info: conv
	@echo "ORIGINALS:"
	identify $(ORIG)
	@echo "CONVERTED:"
	identify $(CONV)

clean:
	rm -Rf ppm *_py1 *_py10

