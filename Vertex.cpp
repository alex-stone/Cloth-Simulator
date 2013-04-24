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

    for(int i = 0; i < 4; i++) {
        stretch[i] = NULL;
        shear[i] = NULL;
        bend[i] = NULL;
    }

    fixed = false;
    mass = 1.0f;  // Arbitrary Value
    springConstant = 40.0f;
    lastTimeUpdated = 0.0f;
}

Vertex::Vertex(float a, float b, float c) {
    this->position = glm::vec3(a, b, c);
    this->velocity = glm::vec3(0.0f);
    this->acceleration = glm::vec3(0.0f);

    stretch = new Vertex*[4];
    shear = new Vertex*[4];
    bend = new Vertex*[4];

    for(int i = 0; i < 4; i++) {
        stretch[i] = NULL;
        shear[i] = NULL;
        bend[i] = NULL;
    }

    fixed = false;
    mass = 1.0f;
    springConstant = 40.0f;
    lastTimeUpdated = 0.0f;
}

Vertex::Vertex(float a, float b, float c, bool isFixed) {
    this->position = glm::vec3(a, b, c);
    this->velocity = glm::vec3(0.0f);
    this->acceleration = glm::vec3(0.0f);

    stretch = new Vertex*[4];
    shear = new Vertex*[4];
    bend = new Vertex*[4];

    for(int i = 0; i < 4; i++) {
        stretch[i] = NULL;
        shear[i] = NULL;
        bend[i] = NULL;
    }
    
    fixed = isFixed;
    mass = 1.0f;
    springConstant = 40.0f;
    lastTimeUpdated = 0.0f;
}

//****************************************************
// Vertex Class - Functions
//****************************************************
void Vertex::setSpringRestLengths(float stretch, float bend, float shear) {
    stretchRestDist = stretch;
    shearRestDist = shear;
    bendRestDist = bend;
}

void Vertex::setFixedVertex(bool isFixed) {
    fixed = isFixed;
}

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
        float time = timestep - lastTimeUpdated;

        position = position + velocity*time;
        velocity = velocity + acceleration * time;

        lastTimeUpdated = timestep;
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
    acceleration = spring + damp + (externalForces);
}


//****************************************************
// GetAccelFromSpring       From a Single Spring 
//      - springVec
//              Direction of Force
//              Length;
//****************************************************
glm::vec3 Vertex::getAccelFromSpring(float restLength, glm::vec3 springVec) {

    // Displacement
    float diff = glm::length(springVec) - restLength;

    // Direction of Force
    glm::vec3 returnVec = glm::normalize(springVec);
    
    // ReturnVec = (k * x) * direction
    returnVec = returnVec * (springConstant * diff) / mass;

    return returnVec;
}

//****************************************************
// GetSpringAccel
//      - Calculates the net Acceleration from all
//        12 of the springs.
//****************************************************
glm::vec3 Vertex::getSpringAccel() {
    // Iterate through each spring connection and calculate force 
    glm::vec3 returnVec;


    for(int i = 0; i < 4; i++) {
        if(stretch[i] != NULL) {
            returnVec += getAccelFromSpring(stretchRestDist, this->vectorTo(stretch[i]));
        }
/*
        if(shear[i] != NULL) {
            returnVec += getAccelFromSpring(shearRestDist, this->vectorTo(shear[i]));
        }

        if(bend[i] != NULL) {
            returnVec += getAccelFromSpring(bendRestDist, this->vectorTo(bend[i]));
        }
  */
    }


    return returnVec;
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


