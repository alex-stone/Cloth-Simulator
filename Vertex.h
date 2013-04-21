#ifndef VERTEX_H
#define VERTEX_H

//****************************************************
// Vertex Header Definition
//****************************************************

class Vertex {
  private:
    // TODO: Vertex Properties:
    // Position - Coordinate
    float x, y, z;

    // Velocity - Vector
    // Acceleration - Vector
    //     Force's Calculated for Each Vertex


    // TODO: Spring Connections:
    // 4 Stretch Springs: Horizontal & Vertical
    // 4 Shear Springs: Diagonal
    // 4 Bend Springs: 2nd Horizontal & Vertical

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
    float getX() { return x; };
    float getY() { return y; };
    float getZ() { return z; };



};

#endif
