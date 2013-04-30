#include <iostream>
#include <math.h>

#include "glm/glm.hpp"

#include "Plane.h"

//****************************************************
// Plane Class Definition
//****************************************************
Plane::Plane(){
    topLeft = glm::vec3(-0.5f, -1.0f, 0.5f);
    topRight = glm::vec3(0.5f, -1.0f, 0.5f);
    lowRight = glm::vec3(0.5f, -1.0f, -0.5f);
    lowLeft = glm::vec3(-0.5f, -1.0f, -0.5f);
    normal = calcNormal();
}

Plane::Plane(glm::vec3 ul, glm::vec3 ur, glm:vec3 lr, glm::vec3 ll) {
    topLeft = ul;
    topRight = ur;
    lowRight = lr;
    lowLeft = ll;
    normal = calcNormal();
}

Plane::Plane(glm::vec3 ul, glm::vec3 vecToRight, glm::vec3 vecToLow) {
    topLeft = ul;
    topRight = ul+vecToRight;
    lowRight = ul+vecToRight + vecToLow;
    lowLeft = ul + vecToLow;
    normal = calcNormal();
}

glm::vec3 Plane::calcNormal() {
    glm::vec3 v1 = lowRight - topLeft;
    glm::vec3 v2 = topRight - topLeft;

    return glm::cross(v1, v2);
}

//****************************************************
// Test if the change in position vector newPos - oldPos, intersects the plane
// Since velocity = currentPos - oldPos, check if vector (- velocity) going from current Position intersects the plane
//
//****************************************************
bool Plane::collide(Vertex* v) {
   
    // Point to Plane Collision
    //      Particle P with Velocity V moving towards Plane with Normal:
    //      Collision occurs if (X - P) dot N < threshold
    //      If N dot V is less than zero colliding, otherwise separating
    //      - If Colliding:
    //          Separate motion into Tangential and Parallel to Normal.
    //      TODO: Perhaps change collision detection to be a part of update.
    //      Thus if a vector collides it doesn't call update as it normally would

    glm::vec3 path = - v->getVel();
      
    // (X - P) dot N < threshold

    // X to any point in the plane dot N, is the distance to the palne

    float distToPlane = glm::dot((v->getPos() - topLeft), normal);
    // TODO: CHECK if negative values are appropriate
    

    if(distToPlane < 0.01) {
        if(glm::dot(normal, v->getVel()) < 0.0f) {
            std::cout << "Collision with Plane" << std::endl;

            glm::vec3 parallel = glm::dot(normal, v->getVel()) * normal;
            glm::vec3 tangential = v->getVel() - parallel;

            // V' = tangential - Kf * parallel
            float kf = 0.1f;

            glm::vec3 newVel = tangential - kf * parallel;
            glm::vec3 newPos = v->getPos() - velocity;

            v->updateAfterCollide(newPos, newVel);
            
            return true; 
        }

    }

    return false;

}



glm::vec3 Sphere::getNormal(Vertex* v) {
    glm::vec3 returnVec = v->getPos() - center;
    return glm::normalize(returnVec);
}


