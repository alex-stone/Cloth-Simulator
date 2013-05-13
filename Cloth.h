#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include "Vertex.h"
#include "Shape.h"

//****************************************************
// Cloth Header Definition
//****************************************************

class Cloth {
  private:
    int width;   // Number of Vertices
    int height;  // Number of Vertices 
    
    // 2D Array of Vertices: vertexMatrix[i][j] == vertexMatrix(i*w + j)
    std::vector<Vertex*> vertexMatrix;

    // TODO: Other Variables
    //      bool tearable
    //      Spring Constants
    //      Dampening Constants

    // Spring Constants - Defined for 1 UNIT Length (1 m)


    float springConstant;

    float stretchConst;
    float shearConst;
    float bendConst;
    
    float dampeningConstant;

    // Integration Type
    bool euler;

    // Private Functions and Constructor Helpers:
    void createDefaultCloth(int w, int h);
    void createVertices(glm::vec3 upLeft, glm::vec3 vertStep, glm::vec3 horizStep); // height and width already instantiated


  public:
    // Constructors:
    Cloth();
    Cloth(int w, int h);
    Cloth(int density, Vertex* upLeft, Vertex* upRight, Vertex* downRight, Vertex* downLeft, bool isEuler);
    Cloth(int w, int h, Vertex* upLeft, Vertex* upRight, Vertex* downRight, Vertex* downLeft, bool isEuler);
    // Other Constructors: Include Spring Constants

    // Getters:
    int getWidth() { return width; };
    int getHeight() { return height; };    

    // Width oriented vector
    Vertex* getVertex(int w, int h) { return vertexMatrix[h*width + w]; };

    void resetAccel();

    // Update Cloth:
    void update(float timestep);
    void updateNormals();
    //void update(float timestep,glm::vec3 spherePosition, float sphereRadius);
    void updateCollision(Shape* s);

    void addConstantAccel(glm::vec3 accel);
    void addTriangleForce(glm::vec3 force);
    /*
    void addExtForce(glm::vec3 force);
    void addExternalForce(glm::vec3 externalForce);
*/
    void setFixedCorners(bool c1, bool c2, bool c3, bool c4);
    void connectSprings();

};

#endif
