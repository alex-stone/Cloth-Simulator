#include <iostream>
#include <math.h>

#include "glm/glm.hpp"

#include "Sphere.h"

//****************************************************
// Sphere Class Definition
//****************************************************
Sphere::Sphere() {
    center = glm::vec3(0.0f, -1.0f, 0.0f);
    radius = 1.0f;
}

Sphere::Sphere(glm::vec3 cent, float rad) {
    center = cent;
    radius = rad;
}

bool Sphere::collide(Vertex* v) {
    
    glm::vec3 temp = v->getPos() - center;
    float n = glm::length(temp);

    if(n < this->radius) {
        // Update Vertex Position & Velocity
        
        glm::vec3 newPos = glm::normalize(temp) * radius + center;
        glm::vec3 newVel(0.4f, 0.4f, 0.4f);

        v->updateAfterCollide(newPos, newVel);

        return true;
    }
    
    return false;
}



glm::vec3 Sphere::getNormal(Vertex* v) {
    glm::vec3 returnVec = v->getPos() - center;
    return glm::normalize(returnVec);
}

