#include <iostream>
#include <math.h>
#include "glm/glm.hpp"

#include "Vertex.h"

// Default Damp Factor
const float DAMP_FACTOR = 0.015f;

bool DEBUG = true ;

//****************************************************
// Vertex Class - Constructors
//****************************************************
Vertex::Vertex() {
    initPhysicalProps(0.0f, 0.0f, 0.0f); 
    
    fixed = false;
    lastTimeUpdated = 0.0f;
}

Vertex::Vertex(float a, float b, float c) {
    initPhysicalProps(a, b, c);

    fixed = false;
    lastTimeUpdated = 0.0f;
}

Vertex::Vertex(float a, float b, float c, bool isFixed) {
    initPhysicalProps(a, b, c);
    
    fixed = isFixed;
    lastTimeUpdated = 0.0f;
}

//****************************************************
// Vertex Class - Private Functions
//****************************************************

void Vertex::initPhysicalProps(float a, float b, float c) {
    this->position = glm::vec3(a, b, c);
    this->velocity = glm::vec3(0.0f);
    this->acceleration = glm::vec3(0.0f);

    this->oldPos = glm::vec3(a, b, c);  // Initializes oldPos to currentPos
    
    mass = 1.0f;

    dampFactor = DAMP_FACTOR;

    //mass = 1.0f;

    firstUpdate = true;
}

//****************************************************
// Vertex Class - Setter Functions
//****************************************************
void Vertex::setPosition(int x, int y) {
    xPos = x;   // X is width
    yPos = y;   // Y is Height
}

void Vertex::setFixedVertex(bool isFixed) {
    fixed = isFixed;
}

void Vertex::setDamp(float newDamp) {
    dampFactor = newDamp;
}

//****************************************************
// Verlet Integration - Update Position      
//****************************************************
void Vertex::updateVerlet(float timeChange) {


    glm::vec3 temp = position;

    position = position + (position - oldPos)*(1.0f - dampFactor) + acceleration * (timeChange * timeChange);

    if(oldTimeChange == 0.0f) {
        oldTimeChange = 0.1f;
    }

    float factor = timeChange / oldTimeChange;

    //std::cout << "Factor = " << factor << std::endl;


    //position = position + (position - oldPos)*(timeChange/oldTimeChange) + acceleration * (timeChange * timeChange);

    oldPos = temp;
    velocity = position - oldPos;
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    oldTimeChange = timeChange;


    // Works but is really slow
   //glm::vec3 newPos = position + (position - oldPos)*(timeChange/oldTimeChange)+ acceleration * (timeChange * timeChange);


    //glm::vec3 newPos = position + (position - oldPos)*(1.0f - DAMP_FACTOR)+ acceleration * (timeChange * timeChange);


    //glm::vec3 newPos = position

    //glm::vec3 newPos = position + (position - oldPos) * (timeChange / oldTimeChange) + acceleration * (timeChange * timeChange);

  //glm::vec3 newPos = (2.0f * position) - oldPos + acceleration * (timeChange * timeChange);


//    glm::vec3 newPos = (float)2 *position - oldPos + acceleration * (timeChange * timeChange);

    /*
    if(xPos == 1 && yPos == 1 && DEBUG) {
        std::cout << std::endl;

        std::cout << "New Timestep = " << timeChange << std::endl;
        printPosition();
        printVelocity();
        printAccel();
    } */

        /*

    oldPos = position;
    position = newPos;
    velocity = position - oldPos;
    oldTimeChange = timeChange; */
}

//****************************************************
// Euler Integration - Update Position
//****************************************************
void Vertex::updateEuler(float timeChange) {

    //position = position + (velocity * timeChange);
    velocity = velocity + (acceleration * timeChange);
    position = position + (velocity * timeChange);
}

//****************************************************
// Force Calculation Functions
//      - Use Euler's Methods to update Position & 
//        Velocity
//****************************************************
void Vertex::update(float timeChange, bool euler) {
    if(firstUpdate) {
        oldTimeChange = timeChange; // Gives a 1.0 ratio
        firstUpdate = false;
    }

  //  updateInternal();

    if(!fixed) {
        if(euler) {
            updateEuler(timeChange);
        } else {
            updateVerlet(timeChange);
        }
    }

    // Make sure this is correct
    this->acceleration = glm::vec3(0,0,0);
}


void Vertex::offsetCorrection(glm::vec3 correctionVec) {
    if(!fixed){
        position += correctionVec;
    }

}

//****************************************************
// Reset Acceleration:
//      - Sets Acceleration to 0.
//****************************************************
void Vertex::resetAccel() {
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
}

//****************************************************
// Add Acceleration:
//      - Used for Constant Acceleration Forces:
//          - I.E. Gravity
//****************************************************
void Vertex::addAccel(glm::vec3 accel) {
    acceleration += accel;
}

//****************************************************
// Add Force:
//      - Adds Force vector to current Vertice's
//        acceleration.
//****************************************************
void Vertex::addForce(glm::vec3 force) {
    acceleration += force / mass;
}

//****************************************************
// Update Acceleration:
//      - Given External Forces, Spring Forces, and
//        Dampening Forces, calculate new acceleration
//****************************************************
void Vertex::updateNormal(glm::vec3 addNorm) {
    this->normal += addNorm;
}


//****************************************************
// Find Normal:
//      - Calculates Normal for the triangle formed by
//        this Vertex, v2 & v3.
//****************************************************
glm::vec3 Vertex::findNormal(Vertex *v2, Vertex *v3){
    glm::vec3 temp_v1 = v2->getPos() - this->getPos();
    glm::vec3 temp_v2 = v3->getPos() - this->getPos();

    return glm::cross(temp_v1,temp_v2);
}


//****************************************************
// Reset Normal:
//      - Sets Normal to 0.
//****************************************************
void Vertex::resetNorm() {
    normal = glm::vec3(0.0f, 0.0f, 0.0f);
}


void Vertex::setNormal(glm::vec3 newNormal) {
    this->normal = newNormal;
}

//****************************************************
// Update After Collide:
//      - Function called by Shape, when a collision
//        is detected.
//      - Vertex acts based on how Shape choose to
//        reposition it.
//****************************************************
void Vertex::updateAfterCollide(glm::vec3 newPos, glm::vec3 newVel) {
    this->position = newPos;
    
    this->velocity = newVel;
/*
    this->velocity.x *= .4;
    this->velocity.y *= .4;
    this->velocity.z *= .4;
    */
    
}

//****************************************************
// Get Damp Force:
//      - Calculates Damp Force based on velocity
//      - TODO: incorporate it into Verlet Integration
//          Formula.
//****************************************************
glm::vec3 Vertex::getDampForce() {
    glm::vec3 returnVec = -velocity * dampFactor;

    return returnVec;
}

//****************************************************
// Vector To:
//      - Returns a vector from This Vertex to other
//        Vertex a.
//****************************************************
glm::vec3 Vertex::vectorTo(Vertex* a) {
    glm::vec3 posA = a->getPos();
    glm::vec3 returnVec = posA - this->position;

    return returnVec;
}

//****************************************************
// Print Helper Functions:
//      - For Displaying Info and Debugging
//****************************************************

void Vertex::printCoordinate() {
    std::cout << "(" << xPos << ", " << yPos << ")" << std::endl;
}

void Vertex::printPosition() {
    std::cout << "(" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void Vertex::printVelocity() {
    std::cout << "(" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
}

void Vertex::printAccel() {
    std::cout << "(" << acceleration.x << ", " << acceleration.y << ", " << acceleration.z << ")" << std::endl;
}

