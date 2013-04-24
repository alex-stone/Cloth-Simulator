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
    this->netForce = glm::vec3(0.0f);
    this->normal= glm::vec3(0.0f);
    this->dampingFactor = 0.5f;
    fixed = false;
    mass = 1.0f;  // Arbitrary Value

}

Vertex::Vertex(float a, float b, float c) {
    this->position = glm::vec3(a,b,c);
    this->velocity = glm::vec3(0.0f);
    this->acceleration = glm::vec3(0.0f);
    this->netForce = glm::vec3(0.0f);
    this->normal= glm::vec3(0.0f);
    this->dampingFactor = 0.5f;
    fixed = false;
    mass = 1.0f;

}

Vertex::Vertex(float a, float b, float c, bool isFixed) {
    this->position = glm::vec3(a, b, c);
    this->velocity = glm::vec3(0.0f);
    this->acceleration = glm::vec3(0.0f);
    this->netForce = glm::vec3(0.0f);
    this->normal= glm::vec3(0.0f);
    this->dampingFactor = 0.5f;
    fixed = isFixed;
    mass = 0.001f;

}

//****************************************************
// Vertex Class - Functions
//****************************************************

void connectStretch(Vertex a, int n) {

}

// Returns the vector that goes from this Vertex to Vertex a.
glm::vec3 Vertex::vectorTo(Vertex* a) {
    glm::vec3 posA = a->getPos();
    glm::vec3 returnVec = posA - this->position;

    return returnVec;
}

void Vertex::updateVertex(float t)
{
    if(t>0 && fixed == false){
        position += velocity * t;
        velocity += ((netForce/mass)*t);
        netForce = glm::vec3(0.0f);
        velocity *= pow(dampingFactor,t);
    }
}



