#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include "Vertex.h"


//****************************************************
// Vertex Class - Constructors
//****************************************************
Vertex::Vertex() {
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;

    fixed = false;
    mass = 0.001f;  // Arbitrary Value

}

Vertex::Vertex(float a, float b, float c) {
    this->x = a;
    this->y = b;
    this->z = c;

    fixed = false;
    mass = 0.001f;

}

Vertex::Vertex(float a, float b, float c, bool isFixed) {
    this->x = a;
    this->y = b;
    this->z = c;

    fixed = isFixed;
    mass = 0.001f;

}

//****************************************************
// Vertex Class - Functions
//****************************************************

void connectStretch(Vertex a, int n) {

}



