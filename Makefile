# Makefile - Cloth Simulation 

CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
        CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX
        LDFLAGS = -framework GLUT -framework OpenGL \
        -L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
        -lGL -lGLU -lm -lstdc++
else
        CFLAGS = -g -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin
        LDFLAGS = -lglut -lGLU
endif


SOURCES = Scene.cpp Vertex.cpp Cloth.cpp Sphere.cpp Plane.cpp Spring.cpp
OBJECTS = Scene.o Vertex.o Cloth.o Sphere.o Plane.o Spring.o


RM = /bin/rm -f
all: main
#main: $(OBJECTS)
#	$(CC) $(CFLAGS) -o bezier bezier.o $(LDFLAGS) 

main: $(OBJECTS)
	$(CC) $(CFLAGS) -o Scene $(OBJECTS) $(LDFLAGS)

Scene.o: Scene.cpp
	$(CC) $(CFLAGS) -c $(SOURCES)

#bezier.o: bezier.cpp
#	$(CC) $(CFLAGS) -c bezier.cpp -o bezier.o


clean: 
	$(RM) *.o Scene

