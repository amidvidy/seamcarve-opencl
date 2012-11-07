APPNAME := seamc

OBJ = main.c.o seamc.c.o

all: $(OBJ)
	gcc -pthread -fopenmp $(OBJ) -o $(APPNAME)

%.c.o: %.c
	gcc -O3 -c $<

%.cpp.o: %.cpp
	gpp -O3 -c $<

clean:
	rm -rf $(APPNAME) *.o
