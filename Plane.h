#ifndef PLANE_H
#define PLANE_H

#include "glm/glm.hpp"
#include "Shape.h"
#include "Vertex.h"

//****************************************************
// Plane Header Definition
//****************************************************

class Plane : public Shape {
  private:
    glm::vec3 topLeft;
    glm::vec3 topRight;
    glm::vec3 lowRight;
    glm::vec3 lowLeft;
    glm::vec3 normal;

    bool isFloor;

  public:
    // Constructors
    Plane();
    Plane(glm::vec3 ul, glm::vec3 ur, glm::vec3 lr, glm::vec3 ll);
    Plane(glm::vec3 ul, glm::vec3 vecToRight, glm::vec3 vecToLow);

    glm::vec3 calcNormal();

    glm::vec3 getUL() { return topLeft; };
    glm::vec3 getUR() { return topRight; };
    glm::vec3 getLR() { return lowRight; };
    glm::vec3 getLL() { return lowLeft; };

    bool isPointInPlane(glm::vec3 pointOnPlane);

    void setFloor() {isFloor = true;};

    // Shape - Abstract Functions
    bool collide(Vertex* v); 
    std::string getType() { return "PLANE"; };
    bool isTypeFloor() { return isFloor; };

    glm::vec3 getNormal() {return normal;};
    
};

#endif
