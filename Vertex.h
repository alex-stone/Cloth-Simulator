#ifndef VERTEX_H
#define VERTEX_H

#include "glm/glm.hpp"

//****************************************************
// Vertex Header Definition
//****************************************************

class Vertex {
  private:
    // TODO: Vertex Properties:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    // TODO: Spring Connections:
    // 4 Stretch Spring: Left, Up, Right, Down
    Vertex* stretch[4];

    // 4 Shear Springs: Up-Left, Up-Right, Down-Right, Down-Left
    Vertex* shear[4];
    
    // 4 Bend Springs: Left, Up, Right, Down
    Vertex* bend[4];

    // TODO: Variables
    bool fixed;
    float mass;

  public:
    // Constructors:
    Vertex();
    // Constructor Options:
    // Point in Space
    // Cloth Vertex Coordinate
    // Connections 
    // Fixed Or Not

    Vertex(float a, float b, float c);
    Vertex(float a, float b, float c, bool isFixed);

    // Getters
    glm::vec3 getPos() { return position; };
    glm::vec3 getVelocity() { return velocity; };
    glm::vec3 getAccel() { return acceleration; };

    // Connect Vertex a, to this Vertex, in direction n
    void connectStretch(Vertex a, int n);
    void connectShear(Vertex a , int n);
    void connectBend(Vertex a, int n);

    // Direction To Vertex:
    glm::vec3 vectorTo(Vertex* a);

    // Force Calculation Functions
    //      External Forces:
    //          - Gravity, Wind, etc...
    //      Internal Forces:
    //          - 12 Spring Connections

};

#endif
