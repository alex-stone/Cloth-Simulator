#include <iostream>
#include <math.h>
#include "glm/glm.hpp"

#include "Vertex.h"

const float DEF_STRETCH = 1000.0f;
const float DEF_SHEAR = 1000.0f;
const float DEF_BEND = 1000.0f;
const float DEF_DAMP = 115.0f;

//****************************************************
// Vertex Class - Constructors
//****************************************************
Vertex::Vertex() {
    initPhysicalProps(0.0f, 0.0f, 0.0f); 
    initSpringToNull(); 
    initSpringConstants(DEF_STRETCH, DEF_SHEAR, DEF_BEND, DEF_DAMP);
    
    fixed = false;
    lastTimeUpdated = 0.0f;
}

Vertex::Vertex(float a, float b, float c) {
    initPhysicalProps(a, b, c);
    initSpringToNull(); 
    initSpringConstants(DEF_STRETCH, DEF_SHEAR, DEF_BEND, DEF_DAMP);

    fixed = false;
    lastTimeUpdated = 0.0f;
}

Vertex::Vertex(float a, float b, float c, bool isFixed) {
    initPhysicalProps(a, b, c);
    initSpringToNull();
    initSpringConstants(DEF_STRETCH, DEF_SHEAR, DEF_BEND, DEF_DAMP);
    
    fixed = isFixed;
    lastTimeUpdated = 0.0f;
}

Vertex::Vertex(float a, float b, float c, float stretchConst, float shearConst, float bendConst) {
    initPhysicalProps(a, b, c);
    initSpringToNull();
    initSpringConstants(stretchConst, shearConst, bendConst, DEF_DAMP);

    fixed = false;
    lastTimeUpdated = 0.0f;
}

Vertex::Vertex(float a, float b, float c, float stretchConst, float shearConst, float bendConst, bool isFixed) {
    initPhysicalProps(a, b, c);
    initSpringToNull();
    initSpringConstants(stretchConst, shearConst, bendConst, DEF_DAMP);

    fixed = isFixed;
    lastTimeUpdated = 0.0f;

}


//****************************************************
// Vertex Class - Private Functions
//****************************************************
void Vertex::initSpringToNull() {
    stretch = new Vertex*[4];
    shear = new Vertex*[4];
    bend = new Vertex*[4];

    for(int i = 0; i < 4; i++) {
        stretch[i] = NULL;
        shear[i] = NULL;
        bend[i] = NULL;
    }
}

void Vertex::initPhysicalProps(float a, float b, float c) {
    this->position = glm::vec3(a, b, c);
    this->velocity = glm::vec3(0.0f);
    this->acceleration = glm::vec3(0.0f);

    this->oldPos = glm::vec3(a, b, c);
<<<<<<< HEAD

    mass = 1.0f;
}

void Vertex::initSpringConstants(float stretchConst, float shearConst, float bendConst, float dampConst) {
    stretchConstant = stretchConst;
    shearConstant = shearConst;
    bendConstant = bendConst;

    dampConstant = dampConst;
}

//****************************************************
// Vertex Class - Setter Functions
//****************************************************
void Vertex::setPosition(int x, int y) {
    xPos = x;   // X is width
    yPos = y;   // Y is Height
}

void Vertex::setSpringRestLengths(float stretch, float bend, float shear) {
    stretchRestDist = stretch;
    shearRestDist = shear;
    bendRestDist = bend;
}

=======

    mass = 1.0f;
}

void Vertex::initSpringConstants(float stretchConst, float shearConst, float bendConst, float dampConst) {
    stretchConstant = stretchConst;
    shearConstant = shearConst;
    bendConstant = bendConst;

    dampConstant = dampConst;
}

//****************************************************
// Vertex Class - Setter Functions
//****************************************************
void Vertex::setPosition(int x, int y) {
    xPos = x;   // X is width
    yPos = y;   // Y is Height
}

void Vertex::setSpringRestLengths(float stretch, float bend, float shear) {
    stretchRestDist = stretch;
    shearRestDist = shear;
    bendRestDist = bend;
}

>>>>>>> Collisions updated
void Vertex::setFixedVertex(bool isFixed) {
    fixed = isFixed;
}

//****************************************************
// Vertex Class - Functions
//****************************************************
void Vertex::connectStretch(Vertex* a, int n) {
    stretch[n] = a;
}
<<<<<<< HEAD

void Vertex::connectShear(Vertex* a, int n) {
    shear[n] = a;
}

void Vertex::connectBend(Vertex* a, int n) {
    bend[n] = a;
}

//****************************************************
// Verlet Integration - Update Position      
//****************************************************
void Vertex::updateVerlet(float timeChange) {

    glm::vec3 newPos = position + velocity + acceleration * (timeChange * timeChange);


//    glm::vec3 newPos = (2.0f * position) - oldPos + acceleration * (timeChange * timeChange);


//    glm::vec3 newPos = (float)2 *position - oldPos + acceleration * (timeChange * timeChange);

    oldPos = position;
    position = newPos;
    velocity = position - oldPos;
}

//****************************************************
// Euler Integration - Update Position
//****************************************************
void Vertex::updateEuler(float timeChange) {

    position = position + (velocity * timeChange);
    velocity = velocity + (acceleration * timeChange);

}

//****************************************************
// Force Calculation Functions
//      - Use Euler's Methods to update Position & 
//        Velocity
//****************************************************
void Vertex::update(float timeChange, bool euler) {
   
    if(!fixed) {
        if(euler) {
            updateEuler(timeChange);
        } else {
            updateVerlet(timeChange);
        }
    }
}

//****************************************************
// Update Acceleration:
//      - Given External Forces, Spring Forces, and
//        Dampening Forces, calculate new acceleration
//****************************************************
void Vertex::updateAccel(glm::vec3 externalForces) {
    glm::vec3 spring = this->getSpringForce();
    glm::vec3 damp = this->getDampForce();


    // Net Acceleration = Sum of 3 vectors
    acceleration = (spring + damp + externalForces) / mass;
}


//****************************************************
// GetAccelFromSpring       From a Single Spring 
//      - springVec
//              Direction of Force
//              Length;
//****************************************************
glm::vec3 Vertex::getForceFromSpring(float restLength, float springConstant, glm::vec3 springVec) {

    // Displacement
    float diff = glm::length(springVec) - restLength;

    // Direction of Force
    glm::vec3 returnVec = glm::normalize(springVec);
    
    // ReturnVec = (k * x) * direction
    returnVec = returnVec * springConstant * diff;

    return returnVec;
}

//****************************************************
// GetSpringAccel
//      - Calculates the net Acceleration from all
//        12 of the springs.
//****************************************************
glm::vec3 Vertex::getSpringForce() {
    // Iterate through each spring connection and calculate force 
    glm::vec3 returnVec;


    for(int i = 0; i < 4; i++) {
        if(stretch[i] != NULL) {
            returnVec += getForceFromSpring(stretchRestDist, stretchConstant, this->vectorTo(stretch[i]));
        }

        if(shear[i] != NULL) {
            returnVec += getForceFromSpring(shearRestDist, shearConstant, this->vectorTo(shear[i]));
        }

        if(bend[i] != NULL) {
            returnVec += getForceFromSpring(bendRestDist, bendConstant, this->vectorTo(bend[i]));
        }
  
    }

    return returnVec;
}

=======

void Vertex::connectShear(Vertex* a, int n) {
    shear[n] = a;
}

void Vertex::connectBend(Vertex* a, int n) {
    bend[n] = a;
}

//****************************************************
// Verlet Integration - Update Position      
//****************************************************
void Vertex::updateVerlet(float timeChange) {

    glm::vec3 newPos = position + velocity + acceleration * (timeChange * timeChange);


//    glm::vec3 newPos = (2.0f * position) - oldPos + acceleration * (timeChange * timeChange);
//    glm::vec3 newPos = (float)2 *position - oldPos + acceleration * (timeChange * timeChange);

    oldPos = position;
    position = newPos;
    velocity = position - oldPos;
}

//****************************************************
// Euler Integration - Update Position
//****************************************************
void Vertex::updateEuler(float timeChange) {

    position = position + (velocity * timeChange);
    velocity = velocity + (acceleration * timeChange);

}

//****************************************************
// Force Calculation Functions
//      - Use Euler's Methods to update Position & 
//        Velocity
//****************************************************
void Vertex::update(float timeChange, bool euler) {
   
    if(!fixed) {
        if(euler) {
            updateEuler(timeChange);
        } else {
            updateVerlet(timeChange);
        }
    }
}

//****************************************************
// Update Acceleration:
//      - Given External Forces, Spring Forces, and
//        Dampening Forces, calculate new acceleration
//****************************************************
void Vertex::updateAccel(glm::vec3 externalForces) {
    glm::vec3 spring = this->getSpringForce();
    glm::vec3 damp = this->getDampForce();


    // Net Acceleration = Sum of 3 vectors
    acceleration = (spring + damp + externalForces) / mass;
}

// Use the cross product of other triangles to calculate. Sum of all the normals of every Triangle
void Vertex::setNormal() {
    


}


void Vertex::updateCollisions(glm::vec3 &center, float r){

        glm::vec3 temp = this->position - center;
        //check if position of vertex - center of ball is less than radius
        float n = glm::length(temp);
        if (n < r)
        {
            
            this->position = glm::normalize(temp)* r + center;
            this->velocity.x *= .4;
            this->velocity.y *= .4;
            this->velocity.z *= .4;
        }
    }



//****************************************************
// GetAccelFromSpring       From a Single Spring 
//      - springVec
//              Direction of Force
//              Length;
//****************************************************
glm::vec3 Vertex::getForceFromSpring(float restLength, float springConstant, glm::vec3 springVec) {

    // Displacement
    float diff = glm::length(springVec) - restLength;

    // Direction of Force
    glm::vec3 returnVec = glm::normalize(springVec);
    
    // ReturnVec = (k * x) * direction
    returnVec = returnVec * springConstant * diff;

    return returnVec;
}

//****************************************************
// GetSpringAccel
//      - Calculates the net Acceleration from all
//        12 of the springs.
//****************************************************
glm::vec3 Vertex::getSpringForce() {
    // Iterate through each spring connection and calculate force 
    glm::vec3 returnVec;


    for(int i = 0; i < 4; i++) {
        if(stretch[i] != NULL) {
            returnVec += getForceFromSpring(stretchRestDist, stretchConstant, this->vectorTo(stretch[i]));
        }

        if(shear[i] != NULL) {
            returnVec += getForceFromSpring(shearRestDist, shearConstant, this->vectorTo(shear[i]));
        }

        if(bend[i] != NULL) {
            returnVec += getForceFromSpring(bendRestDist, bendConstant, this->vectorTo(bend[i]));
        }
  
    }

    return returnVec;
}

>>>>>>> Collisions updated
glm::vec3 Vertex::getDampForce() {
    glm::vec3 returnVec = - velocity * dampConstant;

    return returnVec;
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

void Vertex::printVelocity() {
    std::cout << "(" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
}

void Vertex::printAccel() {
    std::cout << "(" << acceleration.x << ", " << acceleration.y << ", " << acceleration.z << ")" << std::endl;
}

<<<<<<< HEAD

=======
>>>>>>> Collisions updated

