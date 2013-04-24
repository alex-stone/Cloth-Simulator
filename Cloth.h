#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include "Vertex.h"

//****************************************************
// Cloth Header Definition
//****************************************************

class Cloth {
  private:
    int width;   // cloth width
    int height;  // cloth height
    

    // 2D Array of Vertices
    //      Size = W*H
    //      Indexed by: (i*w+j) equal to vertexMatrix[i][j]
    std::vector<Vertex*> vertexMatrix;

           
    // Private Functions:

    // TODO: Other Variables
    //      bool tearable
    //      bool wireframe
    //      - Color, Texture
    //      Spring Constants

  public:
    // Constructors:
    Cloth();
    Cloth(int w, int h);
    Cloth(int w, int h, Vertex* upLeft, Vertex* upRight, Vertex* downRight, Vertex* downLeft);
    // Other Constructors: Include Spring Constants

    // Getters:
    int getWidth() { return width; };
    int getHeight() { return height; };    
    Vertex* getVertex(int i, int j) { return vertexMatrix[i*width + j]; };

};

#endif
