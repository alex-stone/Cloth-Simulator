#ifndef PLANE_H
#define PLANE_H

#include "glm/glm.hpp"
#include "Shape.h"
#include "Vertex.h"

//****************************************************
// Sphere Header Definition
//****************************************************

class Plane : public Shape {
  private:
    glm::vec3 topLeft;
    glm::vec3 topRight;
    glm::vec3 lowRight;
    glm::vec3 lowLeft;
    glm::vec3 normal;

  public:
    // Constructors
    Plane();
    Plane(glm::vec3 ul, glm::vec3 ur, glm::vec3 lr, glm::vec3 ll);
    Plane(glm::vec3 ul, glm::vec3 vecToRight, glm::vec3 vecToLow);

    glm::vec3 calcNormal();

    // Shape - Abstract Functions
    bool collide(Vertex* v); 
    glm::vec3 getNormal(Vertex* v) {return normal;};
    std::string getType() { return "PLANE"; };
    float getRadius() { return NULL; };
    glm::vec3 getCenter() { return NULL; };
};

#endif
