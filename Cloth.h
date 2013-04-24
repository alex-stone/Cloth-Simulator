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
    float dampeningConstant;

  public:
    // Constructors:
    Cloth();
    Cloth(int w, int h);
    Cloth(int w, int h, Vertex* upLeft, Vertex* upRight, Vertex* downRight, Vertex* downLeft);
    // Other Constructors: Include Spring Constants

    // Getters:
    int getWidth() { return width; };
    int getHeight() { return height; };    

    // Width oriented vector
    Vertex* getVertex(int w, int h) { return vertexMatrix[h*width + w]; };

    // Update Cloth:
    void update(float timestep);

    void connectSprings();

};

#endif
