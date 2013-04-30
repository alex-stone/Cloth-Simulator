#ifndef SHAPE_H
#define SHAPE_H

#include "glm/glm.hpp"
#include "Vertex.h"

//****************************************************
// Shape Header Definition
//****************************************************

class Shape {
  public:
    // If Collides return true and updates Vertex, else return false
    virtual bool collide(Vertex* v) = 0;
    virtual std::string getType() = 0;  

    // Functions for Sphere
    virtual float getRadius() { return -1.0f; };
    virtual glm::vec3 getCenter() {return glm::vec3(0.0f, 0.0f, 0.0f);};
    virtual glm::vec3 getNormal(Vertex* v) { return glm::vec3(0.0f, 0.0f, 0.0f);};

    // Functions for Plane
    virtual glm::vec3 getNormal() { return glm::vec3(0.0f, 0.0f, 0.0f); };
    virtual glm::vec3 getUL() { return glm::vec3(0.0f, 0.0f, 0.0f); };
    virtual glm::vec3 getUR(){ return glm::vec3(0.0f, 0.0f, 0.0f); };
    virtual glm::vec3 getLR(){ return glm::vec3(0.0f, 0.0f, 0.0f); };
    virtual glm::vec3 getLL(){ return glm::vec3(0.0f, 0.0f, 0.0f); };
};

#endif
