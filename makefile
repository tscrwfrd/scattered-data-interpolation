CC = gcc

FLAGS   = -std=c99 -c -fPIC -g -DPRINT -Wall -include ../libs/qhull-2020.2/src/libqhull_r/qhull_ra.h
LFLAGS  = -L../libs/qhull-2020.2/lib -lqhull_r -lqhullstatic_r -lqhullstatic -lm
#OUTPUT  = bin/libgriddata.so
OUTPUT  = sdi.exe
OBJECTS = \
	obj/interpolation.o \
	obj/main.o

all: $(OBJECTS)
	mkdir -p bin
	mkdir -p obj
	$(CC) -g $(OBJECTS) -o $(OUTPUT) $(LFLAGS)
#	$(CC) -g $(OBJECTS) -shared -o $(OUTPUT) $(LFLAGS)

obj/main.o: src/main.c
	mkdir -p bin
	mkdir -p obj
	$(CC) $(FLAGS) src/main.c -o obj/main.o

obj/interpolation.o: src/interpolation.c
	mkdir -p bin
	mkdir -p obj
	$(CC) $(FLAGS) src/interpolation.c -o obj/interpolation.o

clean:
	rm -Rf $(OUTPUT) $(OBJECTS)



