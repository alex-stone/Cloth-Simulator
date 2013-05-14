# Makefile - Cloth Simulation 

CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
        CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX
        LDFLAGS = -framework GLUT -framework OpenGL \
        -L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
        -lGL -lGLU -lm -lstdc++ -L./ -lfreeimage
else
        CFLAGS = -g -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin
        LDFLAGS = -lglut -lGLU -L./ -lfreeimage
endif


SOURCES = Main.cpp Scene.cpp Vertex.cpp Cloth.cpp Sphere.cpp Plane.cpp Spring.cpp
OBJECTS = Main.o Scene.o Vertex.o Cloth.o Sphere.o Plane.o Spring.o


RM = /bin/rm -f
all: main

main: $(OBJECTS)
	$(CC) $(CFLAGS) -o Main $(OBJECTS) $(LDFLAGS)

Main.o: main.cpp
	$(CC) $(CFLAGS) -c $(SOURCES)

clean: 
	$(RM) *.o Main 

