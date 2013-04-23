#ifndef CLOTH_H
#define CLOTH_H

#include "Vertex.h"

//****************************************************
// Cloth Header Definition
//****************************************************

class Cloth {
  private:
    int width;   // Number of Vertices
    int height;  // Number of Vertices 

    // 2D Arrray of Vertices:
    Vertex** vertices;
           
    // Private Functions:
//    initializeDefaultVertices();
    void initializeTestCloth(int w, int h);

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
    Vertex getVertex(int i, int j) { return vertices[width][height]; };

};

#endif
