#include <iostream>
#include <math.h>
#include "Cloth.h"
#include "Vertex.h"

//****************************************************
// Cloth Class - Constructors
//****************************************************

Cloth::Cloth() {
    this->width = 10;
    this->height = 10;

    initializeTestCloth();

}

Cloth::Cloth(int w, int h) {
    this->width = w;
    this->height = h;

    initializeTestCloth();
}


Cloth::initializeTestCloth() {
    vertices = new Vertex[width][height];

    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            if(i == 0 || j == 0 || i == width || j == width) {

                vertices = new Vertex((float)i, 0, (float) k, true);
            } else {
                vertices = new Vertex((float)i, 0, (float) k, false);
            }
        }
    }

    // Connect all the Springs

}


