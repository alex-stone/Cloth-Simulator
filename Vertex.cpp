#include <iostream>
#include <math.h>
#include "glm/glm.hpp"

#include "Vertex.h"


//****************************************************
// Vertex Class - Constructors
//****************************************************
Vertex::Vertex() {
    this->position = glm::vec3(0.0f);
    this->velocity = glm::vec3(0.0f);
    this->acceleration = glm::vec3(0.0f);
    
    stretch = new Vertex*[4];
    shear = new Vertex*[4];
    bend = new Vertex*[4];

    fixed = false;
    mass = 0.001f;  // Arbitrary Value
}

Vertex::Vertex(float a, float b, float c) {
    this->position = glm::vec3(a, b, c);
    this->velocity = glm::vec3(0.0f);
    this->acceleration = glm::vec3(0.0f);

    stretch = new Vertex*[4];
    shear = new Vertex*[4];
    bend = new Vertex*[4];

    fixed = false;
    mass = 0.001f;
}

Vertex::Vertex(float a, float b, float c, bool isFixed) {
    this->position = glm::vec3(a, b, c);
    this->velocity = glm::vec3(0.0f);
    this->acceleration = glm::vec3(0.0f);

    stretch = new Vertex*[4];
    shear = new Vertex*[4];
    bend = new Vertex*[4];

    fixed = isFixed;
    mass = 0.001f;
}

//****************************************************
// Vertex Class - Functions
//****************************************************

void Vertex::connectStretch(Vertex* a, int n) {
    stretch[n] = a;
}

void Vertex::connectShear(Vertex* a, int n) {
    shear[n] = a;
}

void Vertex::connectBend(Vertex* a, int n) {
    bend[n] = a;
}

//****************************************************
// Force Calculation Functions
//      - Use Euler's Methods to update Position & 
//        Velocity
//****************************************************
void Vertex::update(float timestep) {
    // x(i+1) = x(i) + v(i) * dT
    if(!fixed) {
        position = position + velocity*timestep;
        velocity = velocity + acceleration * timestep;
    }
}

//****************************************************
// Update Acceleration:
//      - Given External Forces, Spring Forces, and
//        Dampening Forces, calculate new acceleration
//****************************************************
void Vertex::updateAccel(glm::vec3 externalForces) {
    glm::vec3 spring = this->getSpringAccel();
    glm::vec3 damp = this->getDampAccel();


    // Net Acceleration = Sum of 3 vectors
    acceleration = spring + damp + externalForces;
}

//****************************************************
// GetSpringAccel
//      - Calculates the net Acceleration from all
//        12 of the springs.
//****************************************************
glm::vec3 Vertex::getSpringAccel() {
    // Iterate through each spring connection and calculate force 
    
    glm::vec3 temp(0, 0, 0);

    return temp;
}

glm::vec3 Vertex::getDampAccel() {
    glm::vec3 temp(0, 0, 0);

    return temp;

}


// Returns the vector that goes from this Vertex to Vertex a.
glm::vec3 Vertex::vectorTo(Vertex* a) {
    glm::vec3 posA = a->getPos();
    glm::vec3 returnVec = posA - this->position;

    return returnVec;
}


void Vertex::printPosition() {
    std::cout << "(" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;

}


