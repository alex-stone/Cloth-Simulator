#include <iostream>
#include <math.h>
#include <vector>
#include "glm/glm.hpp"

#include "Cloth.h"
#include "Vertex.h"

//****************************************************
// Cloth Class - Constructors
//****************************************************

Cloth::Cloth() {
    this->width = 4;
    this->height = 4;

}

Cloth::Cloth(int w, int h) {
    this->width = w;
    this->height = h;

}


//****************************************************
// Cloth Constructor called from Parser
//          - Takes the 4 corners, and divides Cloth
//            Into W x H vertices
//
//****************************************************
Cloth::Cloth(int w, int h, Vertex* upLeft, Vertex* upRight, Vertex* downRight, Vertex* downLeft) {
   // Vertex **vertices = new Vertex*[w];
  
    // Sets the size of the vector to W*H 
    vertexMatrix.resize(w * h); 
    
    

    glm::vec3 vertVec = upLeft->vectorTo(downLeft);
    glm::vec3 horizVec = upLeft->vectorTo(upRight);

    glm::vec3 vertStep = vertVec / (float)(h-1);
    glm::vec3 horizStep = horizVec / (float)(w-1);
  
    
    // Iterate through and create each Vertex
    for(int i = 0; i < w; i++) {
        for(int j = 0; j < h; j++) {
            glm::vec3 temp = upLeft->getPos() + ((float)i * horizVec) + ((float)j * vertVec);
            
            //I*W + j indexes vertexMatrix like a 2D array vertexMatrix[i][j]; 
            vertexMatrix[i*w + j] = new Vertex(temp.x, temp.y, temp.z);
             
        }
    }

    

}

/*
void Cloth::initializeTestCloth(int w, int h) {
    vertices = new Vertex[w][h];
    
    for(int i = 0; i < this->width; i++) {
        for(int j = 0; j < this->height; j++) {
            if(i == 0 || j == 0 || i == this->width || j == this->height) {

                vertices[i][j] = new Vertex((float)i, 0, (float) j, true);
            } else {
                vertices[i][j] = new Vertex((float)i, 0, (float) j, false);
            }
        }
    }

    // Connect all the Springs

}
*/

