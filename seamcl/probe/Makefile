
all: probe

probe: probe.cpp
	g++ $< -lOpenCL -o $@

clean:
	rm -f probe

.PHONY: all clean fresh
