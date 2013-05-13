#include <iostream>
#include <math.h>
#include "glm/glm.hpp"

#include "Vertex.h"

/*
// Spring Constants for 1meter, i.e. 1 unit length of 
const float DEF_STRETCH = 1000.0f;
const float DEF_SHEAR = 1000.0f;
const float DEF_BEND = 1000.0f;
const float DEF_DAMP = 100.0f;
*/
// New Useful Variables:
const float DAMP_FACTOR = 0.004f;

bool DEBUG = true ;

//****************************************************
// Vertex Class - Constructors
//****************************************************
Vertex::Vertex() {
    initPhysicalProps(0.0f, 0.0f, 0.0f); 
    //initSpringToNull(); 
    //initSpringConstants(DEF_STRETCH, DEF_SHEAR, DEF_BEND, DEF_DAMP);
    
    fixed = false;
    lastTimeUpdated = 0.0f;
}

Vertex::Vertex(float a, float b, float c) {
    initPhysicalProps(a, b, c);
    //initSpringToNull(); 
   // initSpringConstants(DEF_STRETCH, DEF_SHEAR, DEF_BEND, DEF_DAMP);

    fixed = false;
    lastTimeUpdated = 0.0f;
}

Vertex::Vertex(float a, float b, float c, bool isFixed) {
    initPhysicalProps(a, b, c);
    //initSpringToNull();
   // initSpringConstants(DEF_STRETCH, DEF_SHEAR, DEF_BEND, DEF_DAMP);
    
    fixed = isFixed;
    lastTimeUpdated = 0.0f;
}

/*Vertex::Vertex(float a, float b, float c, float stretchConst, float shearConst, float bendConst) {
    initPhysicalProps(a, b, c);
    initSpringToNull();
    //initSpringConstants(stretchConst, shearConst, bendConst, DEF_DAMP);

    fixed = false;
    lastTimeUpdated = 0.0f;
}

Vertex::Vertex(float a, float b, float c, float stretchConst, float shearConst, float bendConst, bool isFixed) {
    initPhysicalProps(a, b, c);
    initSpringToNull();
    //initSpringConstants(stretchConst, shearConst, bendConst, DEF_DAMP);

    fixed = isFixed;
    lastTimeUpdated = 0.0f;

}*/


//****************************************************
// Vertex Class - Private Functions
//****************************************************
/*
void Vertex::initSpringToNull() {
    stretch = new Vertex*[4];
    shear = new Vertex*[4];
    bend = new Vertex*[4];

    for(int i = 0; i < 4; i++) {
        stretch[i] = NULL;
        shear[i] = NULL;
        bend[i] = NULL;
    }
}*/

void Vertex::initPhysicalProps(float a, float b, float c) {
    this->position = glm::vec3(a, b, c);
    this->velocity = glm::vec3(0.0f);
    this->acceleration = glm::vec3(0.0f);

    this->oldPos = glm::vec3(a, b, c);  // Initializes oldPos to currentPos
    mass = 1.0f;

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
/*
void Vertex::setSpringRestLengths(float stretch, float bend, float shear) {
    stretchRestDist = stretch;
    shearRestDist = shear;
    bendRestDist = bend;
}

void Vertex::initSpringConstants(float stretchConst, float shearConst, float bendConst, float dampConst) {
    stretchConstant = stretchConst;
    shearConstant = shearConst;
    bendConstant = bendConst;

    dampConstant = dampConst;
}*/

void Vertex::setFixedVertex(bool isFixed) {
    fixed = isFixed;
}


//****************************************************
// Vertex Class - Functions
//****************************************************
/*

void Vertex::connectStretch(Vertex* a, int n) {
    stretch[n] = a;
}

void Vertex::connectShear(Vertex* a, int n) {
    shear[n] = a;
}

void Vertex::connectBend(Vertex* a, int n) {
    bend[n] = a;
}*/

//****************************************************
// Verlet Integration - Update Position      
//****************************************************
void Vertex::updateVerlet(float timeChange) {


    glm::vec3 temp = position;

    position = position + (position - oldPos)*(1.0f - DAMP_FACTOR) + acceleration * (timeChange * timeChange);

    if(oldTimeChange == 0.0f) {
        oldTimeChange = 0.1f;
    }

    float factor = timeChange / oldTimeChange;

    //std::cout << "Factor = " << factor << std::endl;


    //position = position + (position - oldPos)*(timeChange/oldTimeChange) + acceleration * (timeChange * timeChange);

    oldPos = temp;
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
// Update Internal Forces:
//      - Update Acceleration given Internal Forces
//****************************************************

/*
void Vertex::updateInternal() {
    glm::vec3 spring = this->getSpringForce();
    //glm::vec3 damp = this->getDampForce();
    //glm::vec3 internalAccel = (spring + damp) / mass;

    glm::vec3 internalAccel = spring / mass;

    acceleration += internalAccel;

    if(xPos == 1 && yPos == 1 && DEBUG) {
        std::cout << "Internal Acceleration " << "(" << internalAccel.x << ", " << internalAccel.y << ", " << internalAccel.z << ")" << std::endl;

    } 


    // Net Acceleration = Sum of 3 vectors
    //acceleration = (spring + damp + externalForces) / mass;
} */

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
// GetAccelFromSpring       From a Single Spring 
//      - springVec
//              Direction of Force
//              Length;
//****************************************************

/*
glm::vec3 Vertex::getForceFromSpring(float restLength, float springConstant, Vertex* v2) {//glm::vec3 springVec) {

    // Displacement
    glm::vec3 springVec = this->vectorTo(v2);

    float dist = glm::length(springVec);

    float diff = (dist - restLength) / dist;

    // Direction of Force
    glm::vec3 returnVec = glm::normalize(springVec);
    
    // ReturnVec = (k * x) * direction
    returnVec = returnVec * springConstant * diff;

    // Stiffness Proportional damping

    float kd = 0.5*springConstant;

    float len = glm::length(springVec);

    glm::vec3 damp = - kd * (springVec) * (glm::dot(springVec, springVec)) / (len * len);

    // Mass Proportional Damping

    glm::vec3 springDir = glm::normalize(springVec);
    glm::vec3 vel1 = springDir * glm::dot(velocity, springDir);
    
    glm::vec3 vel2 = springDir * glm::dot(v2->getVelocity(), springDir);


 //   glm::vec3 damp = (-.5f) * (vel1 - vel2);

    if(glm::length(damp) > 0.8 * glm::length(returnVec)) {
        damp = glm::normalize(damp) * 0.5f* glm::length(returnVec);

    }


    //returnVec += damp;

    return returnVec;
}*/

//****************************************************
// GetSpringAccel
//      - Calculates the net Acceleration from all
//        12 of the springs.
//****************************************************

/*
glm::vec3 Vertex::getSpringForce() {
    // Iterate through each spring connection and calculate force 

    glm::vec3 stretchVec, shearVec, bendVec;
    //glm::vec3 returnVec(0.0f, 0.0f, 0.0f);

    for(int i = 0; i < 4; i++) {
        if(stretch[i] != NULL) {
            stretchVec += getForceFromSpring(stretchRestDist, stretchConstant, stretch[i]); //this->vectorTo(stretch[i]));
        }

        if(shear[i] != NULL) {
            shearVec += getForceFromSpring(shearRestDist, shearConstant, shear[i]);//this->vectorTo(shear[i]));
        }

        if(bend[i] != NULL) {
            bendVec += getForceFromSpring(bendRestDist, bendConstant, bend[i]);//this->vectorTo(bend[i]));
        }
  
    }

    return stretchVec + shearVec + bendVec;
} */

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
    
    //this->velocity = newVel;

    this->velocity.x *= .6;
    this->velocity.y *= .6;
    this->velocity.z *= .6;
    
}

//****************************************************
// Get Damp Force:
//      - Calculates Damp Force based on velocity
//      - TODO: incorporate it into Verlet Integration
//          Formula.
//****************************************************
glm::vec3 Vertex::getDampForce() {
    glm::vec3 returnVec = -velocity * dampConstant;

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

