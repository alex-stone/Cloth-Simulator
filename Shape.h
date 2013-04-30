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
    virtual float getRadius() = 0;
    virtual glm::vec3 getCenter() = 0;
};

#endif
