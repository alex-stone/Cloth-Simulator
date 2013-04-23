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
    // 4 Stretch Spring: Left, Up, Right, Down
    Vertex* stretch1, stretch2, stretch3, stretch4;

    // 4 Shear Springs: Up-Left, Up-Right, Down-Right, Down-Left
    Vertex* shear1, shear2, shear3, shear4;
    
    // 4 Bend Springs: Left, Up, Right, Down
    Vertex* bend1, bend2, bend3, bend4;


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


    void connectStretch(Vertex a);
    void connectShear(Vertex a);
    void connectBend(Vertex a);

};

#endif
