#Requires ImageMagick on the path (convert)

SUFX ?= tif png gif jpg
CARV ?= 1 10 50

ORIG ?= $(wildcard $(foreach sufx,$(SUFX),ORIG/*.$(sufx)))
CONV ?= $(ORIG:ORIG/%=CONV/%.ppm)

IMGS ?= $(CONV)
#IMGS ?= $(sort $(wildcard CONV/*.ppm) $(CONV))
DIRS ?= $(foreach carv,$(CARV),$(IMGS:CONV/%.ppm=%_py$(carv)))


default: info

all: conv $(DIRS)

fresh: clean all

conv: $(CONV)

CONV/%.ppm: ORIG/% | CONV
	convert -strip $< -colorspace RGB -depth 8 $@

%_py1: CONV/%.ppm | carve.py
	-rm -Rf $@
	( mkdir -p $@ && cd $@ && ../carve.py ../$< 1 )

%_py10: CONV/%.ppm | carve.py
	-rm -Rf $@
	( mkdir -p $@ && cd $@ && ../carve.py ../$< 10 )

%_py50: CONV/%.ppm | carve.py
	-rm -Rf $@
	( mkdir -p $@ && cd $@ && ../carve.py ../$< 50 )

CONV:
	mkdir -p CONV

.PHONY: clean info diag

diag:
	@echo $(ORIG)
	@echo $(CONV)
	@echo $(IMGS)
	@echo $(DIRS)
 
info: conv
	@echo "ORIGINALS:"
	identify $(ORIG)
	@echo "CONVERTED:"
	identify $(CONV)

clean:
	rm -Rf CONV *_py1 *_py10

