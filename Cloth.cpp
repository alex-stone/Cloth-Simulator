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

    initializeDefaultVertices();
}

Cloth::Cloth(int w, int h) {
    this->width = w;
    this->height = h;
}


Cloth::initializeDefaultVertices() {
    vertices = new Vertex[width][height];

    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {

            vertices[i][j] = Vertex::getVertex(this, i, j);

        }
    }

}


