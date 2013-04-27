#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include "Vertex.h"

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
    float springConstant;

    float stretchConstant;
    float shearConstant;
    float bendConstant;
    
    float dampeningConstant;

    // Integration Type
    bool euler;

  public:
    // Constructors:
    Cloth();
    Cloth(int w, int h);
    Cloth(int w, int h, Vertex* upLeft, Vertex* upRight, Vertex* downRight, Vertex* downLeft, bool isEuler);
    // Other Constructors: Include Spring Constants

    // Getters:
    int getWidth() { return width; };
    int getHeight() { return height; };    

    // Width oriented vector
    Vertex* getVertex(int w, int h) { return vertexMatrix[h*width + w]; };

    // Update Cloth:
    void update(float timestep,glm::vec3 spherePosition, float sphereRadius);

    void addExternalForce(glm::vec3 externalForce);

    void setFixedCorners(bool c1, bool c2, bool c3, bool c4);
    void connectSprings();

};

#endif