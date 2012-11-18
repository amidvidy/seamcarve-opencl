SUFX ?= tif png gif jpg
CARV ?= 1 10

IMGS ?= $(sort $(wildcard $(foreach sufx,$(SUFX),*.$(sufx))))
DIRS ?= $(foreach carv,$(CARV),$(IMGS:%=%_$(carv)))

DIRS1 = $(DIRS:%=py1_%)

all: $(DIRS1)

py1_%_1: carve.py
	( rm -Rf $@ && mkdir -p $@ && cd $@ && ../carve.py ../$(subst _1,,$(subst py1_,,$@)) 1 )

py1_%_10: carve.py
	( rm -Rf $@ && mkdir -p $@ && cd $@ && ../carve.py ../$(subst _10,,$(subst py1_,,$@)) 10 )


.PHONY: clean info

info: $(IMGS)
	identify $^

clean:
	rm -Rf py1_*

