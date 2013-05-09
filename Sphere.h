#ifndef SPHERE_H
#define SPHERE_H

#include "glm/glm.hpp"
#include "Shape.h"
#include "Vertex.h"

//****************************************************
// Sphere Header Definition
//****************************************************

class Sphere : public Shape {
  private:
    glm::vec3 center;
    float radius;

  public:
    // Constructors
    Sphere();
    Sphere(glm::vec3 cent, float rad);

    // Shape - Abstract Functions
    bool collide(Vertex* v); 
    glm::vec3 getNormal(Vertex* v);
    std::string getType() { return "SPHERE"; };
    float getRadius() { return radius; };
    glm::vec3 getCenter() { return center; };
    glm::vec3 getNormal() { return glm::vec3(1.0f, 0.0f, 0.0f); };
};

#endif
