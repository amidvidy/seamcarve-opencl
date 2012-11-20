#Requires ImageMagick on the path (convert)

SUFX ?= tif png gif jpg ppm pnm pgm
CARV ?= 1 10 50

ORIG ?= $(wildcard $(foreach sufx,$(SUFX),ORIG/*.$(sufx)))
CONV ?= $(ORIG:ORIG/%=CONV/%.ppm)

IMGS ?= $(CONV)
#IMGS ?= $(sort $(wildcard CONV/*.ppm) $(CONV))
DIRS ?= $(foreach carv,$(CARV),$(IMGS:CONV/%.ppm=%_py$(carv)))

PYc := $(DIRS:%=%/carves)
PYo := $(DIRS:%=%/out.ppm)


default: info

all: conv py

fresh: clean all

conv: $(CONV)

CONV/%.ppm: ORIG/% | CONV
	convert -strip $< -colorspace RGB -depth 8 -comment "From $<" $@
	touch --reference=$< $@

py: $(PYc) # Not worried about output image right now $(PYo)

%_py1/carves %_py1/out.ppm: CONV/%.ppm | carve.py
	-rm -Rf $(dir $@)
	( mkdir -p $(dir $@) && cd $(dir $@) && ../carve.py ../$< 1 )

%_py10/carves %_py10/out.ppm: CONV/%.ppm | carve.py
	-rm -Rf $(dir $@)
	( mkdir -p $(dir $@) && cd $(dir $@) && ../carve.py ../$< 10 )

%_py50/carves %_py50/out.ppm: CONV/%.ppm | carve.py
	-rm -Rf $(dir $@)
	( mkdir -p $(dir $@) && cd $(dir $@) && ../carve.py ../$< 50 )


CONV:
	mkdir -p CONV

.PHONY: clean info diag

diag:
	@echo $(ORIG)
	@echo $(CONV)
	@echo $(IMGS)
	@echo $(DIRS)
	@echo $(PYc)
 
info: conv
	@echo "ORIGINALS:"
	identify $(ORIG)
	@echo "CONVERTED:"
	identify $(CONV)

clean:
	rm -Rf CONV *_py1 *_py10 *_py50

