#ifndef SPHERE_H
#define SPHERE_H

#include "glm/glm.hpp"

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

}


