#include <iostream>
#include <math.h>
#include <vector>
#include "glm/glm.hpp"

#include "Cloth.h"
#include "Vertex.h"


//****************************************************
// Cloth Class - Constans
//****************************************************
const float UNIT_STRETCH = 100.0f;
const float UNIT_SHEAR = 100.0f;
const float UNIT_BEND = 50.0f;


//****************************************************
// Cloth Class - Constructors
//****************************************************
Cloth::Cloth() {
    euler = true;

    createDefaultCloth(20, 20);
}

Cloth::Cloth(int w, int h) {
    euler = true;

    createDefaultCloth(w, h);
}

//****************************************************
// Cloth Constructor:
//      - Takes 4 points and a density value
//      - The density value indicates the # of divisions
//          that will occur on the shorter edge of the
//          Cloth
//      - This ensures that in rectangular cloth's the
//          grid is still made up of squares
//      - Also determines the spring constants based
//          on the length of the divisions
//****************************************************
Cloth::Cloth(int density, Vertex* upLeft, Vertex* upRight, Vertex* downRight, Vertex* downLeft, bool isEuler) {

    // Determine which edge is shorter:
    glm::vec3 horizVec = upLeft->vectorTo(upRight);
    glm::vec3 vertVec = upLeft->vectorTo(downLeft);

    float horizLength = glm::length(horizVec);
    float vertLength = glm::length(vertVec);

    if(horizLength < vertLength) {
        this->width = density;
        this->height = (int) ((density * vertLength) / horizLength);

    } else {
        this->height = density;
        this->width = (int) ((density * horizLength) / vertLength);
    }

    // Sets size of the Vector holding the vertices to W * H
    vertexMatrix.resize(this->width * this->height);
    euler = isEuler;

    horizVec = horizVec / (float)(this->width - 1);
    vertVec = vertVec / (float)(this->height - 1);

    // Create the vertices and Connect all the Springs
    createVertices(upLeft->getPos(), vertVec, horizVec);
    connectSprings();

}


//****************************************************
// Cloth Constructor called from Parser
//          - Takes the 4 corners, and divides Cloth
//            Into W x H vertices
//
//****************************************************
Cloth::Cloth(int w, int h, Vertex* upLeft, Vertex* upRight, Vertex* downRight, Vertex* downLeft, bool isEuler) {
    // Sets the size of the vector to W*H 
    vertexMatrix.resize(w * h); 

    // Set Dimensions of Cloth
    this->width = w;
    this->height = h;

    euler = isEuler;

    glm::vec3 vertVec = upLeft->vectorTo(downLeft);
    glm::vec3 horizVec = upLeft->vectorTo(upRight);

    glm::vec3 vertStep = vertVec / (float)(h-1);
    glm::vec3 horizStep = horizVec / (float)(w-1);

    createVertices(upLeft->getPos(), vertStep, horizStep);

}

//****************************************************
// Cloth Constructor Helpers:
//****************************************************
void Cloth::createDefaultCloth(int w, int h) {
    this->width = w;
    this->height = h;

    glm::vec3 upLeft(-1, 0, 1);
    glm::vec3 horizStep(2, 0, 0);
    glm::vec3 vertStep(0, 0, -2);

    horizStep = horizStep / (float) (w - 1);
    vertStep = vertStep / (float) (h - 1);

    createVertices(upLeft, vertStep, horizStep);
    connectSprings();
}

//****************************************************
// Create Vertices:
//      - Sets up the vertices, based on the top left
//          point, and the vector that goes to the
//          next vertex, both vertically & horizontally
//      - Width & Height already determined
//****************************************************
void Cloth::createVertices(glm::vec3 upLeft, glm::vec3 vertStep, glm::vec3 horizStep) {

    // Set Spring Rest Lengths   
    float stretchLength = glm::length(horizStep);
    float bendLength = 2 * stretchLength;
    float shearLength = sqrt(2 * stretchLength * stretchLength);

    std::cout << "STRETCH LENGTH = " << stretchLength << std::endl;

    stretchConst = UNIT_STRETCH / stretchLength;
    shearConst = UNIT_SHEAR / shearLength;
    bendConst = UNIT_BEND / bendLength;

    // Iterate through and create each Vertex
    for(int h = 0; h < this->height; h++) {
        for(int w = 0; w < this->width; w++) {

            //I*W + j indexes vertexMatrix like a 2D array vertexMatrix[i][j]; 
            int vertIndex = h * (this->width) + w;

            glm::vec3 temp = upLeft + ((float)h * vertStep) + ((float)w * horizStep);
            
            vertexMatrix[vertIndex] = new Vertex(temp.x, temp.y, temp.z, stretchConst, shearConst, bendConst);
            vertexMatrix[vertIndex]->setSpringRestLengths(stretchLength, bendLength, shearLength); 

            // Sets its position in the Grid of the cloth
            vertexMatrix[vertIndex]->setPosition(w, h);
        }
    }
}

//****************************************************
// Reset Acceleration:
//      - Resets Acceleration for all Vertices to 0.
//****************************************************
void Cloth::resetAccel() {
    for(int i = 0; i < height*width; i++) {
        vertexMatrix[i]->resetAccel();
    }
}

//****************************************************
// Update Collision:
//      - Iterates through each vertex and tests if it
//          intersects with the shape
//****************************************************
void Cloth::updateCollision(Shape* s) {
    for(int i = 0; i < height*width; i++) {
        s->collide(vertexMatrix[i]);
    }
}

//****************************************************
// Update Normals:
//      - Iterates through each square of the grid of
//          Vertices and for each of the 2 triangles
//          in the square computes the normal.
//      - Adds the normal to the Vertex, so normal is
//          a weighted average of all of the its
//          surrounding triangles
//****************************************************
void Cloth::updateNormals() {
    for(int h = 0; h < this->height - 1; h++) {
        for(int w = 0; w < this->width - 1; w++) {
            Vertex* v1 = getVertex(h,w);
            Vertex* v2 = getVertex(h+1, w);
            Vertex* v3 = getVertex(h, w+1);
            Vertex* v4 = getVertex(h+1, w+1);

            glm::vec3 triNormal1 = glm::normalize(v2->findNormal(v1,v3));

            v1->updateNormal(triNormal1);
            v2->updateNormal(triNormal1);
            v3->updateNormal(triNormal1);

            glm::vec3 triNormal2 = glm::normalize(v4->findNormal(v2,v3));
            
            v2->updateNormal(triNormal2);
            v3->updateNormal(triNormal2);
            v4->updateNormal(triNormal2);
        }
    }
}

//****************************************************
// Add Triangle Force:
//      - Add's a Force that acts on each triangle
//          as opposed to each vertex
//      - Iterates through each triangle and calculates
//          the force, and adds that to each vertex
//          of the triangle
//      - TODO: Weight Force (1/3) ?
//****************************************************
void Cloth::addTriangleForce(glm::vec3 force){
    for(int h = 0; h < this->height - 1; h++) {
        for(int w = 0; w < this->width - 1; w++) {
            Vertex* v1 = getVertex(h,w);
            Vertex* v2 = getVertex(h+1, w);
            Vertex* v3 = getVertex(h, w+1);
            Vertex* v4 = getVertex(h+1, w+1);

            // Calculates Force Contribution on the first Triangle
            glm::vec3 triNormal1 = glm::normalize(v2->findNormal(v1,v3));
            glm::vec3 triForce1 = triNormal1 * glm::dot(triNormal1, force);

            v1->addForce(triForce1);
            v2->addForce(triForce1);
            v3->addForce(triForce1);

            // Calculates Force Contribution on the Second Triangle
            glm::vec3 triNormal2 = glm::normalize(v4->findNormal(v2,v3));
            glm::vec3 triForce2 = triNormal2 * glm::dot(triNormal2, force);

            v2->addForce(triForce2);
            v3->addForce(triForce2);
            v4->addForce(triForce2);

        }
    }
}


//****************************************************
// Add Constant Accel:
//      - Adds a constant force to each of the cloth's
//          vertices. I.E. Gravity
//****************************************************
void Cloth::addConstantAccel(glm::vec3 accel) {
    
    for(int i = 0; i < height*width; i++) {
       // vertexMatrix[i]->updateAccel(externalForce);
        vertexMatrix[i]->addAccel(accel);
    }

}


//****************************************************
// Update:
//      - Calls update on each vertex for given
//          timestep
//      - TODO: Separate Spring Class, then do
//          satisfy constraints after update.
//****************************************************
void Cloth::update(float timestep) {
    // Iterate through vertexMatrix, and update each individual particle

    for(int i = 0; i < height*width; i++) {
        vertexMatrix[i]->update(timestep, euler);
    }

}

//****************************************************
// Set Fixed Corners:
//      - Sets certain corners to be fixed in space /
//          unmovable
//****************************************************
void Cloth::setFixedCorners(bool c1, bool c2, bool c3, bool c4) {
    
    if(c1) {
        this->getVertex(0, 0)->setFixedVertex(true);
    }

    if(c2) {
        this->getVertex(width-1, 0)->setFixedVertex(true);
    } 

    if(c3) {
        this->getVertex(width-1, height-1)->setFixedVertex(true);
    }

    if(c4) {
        this->getVertex(0, height-1)->setFixedVertex(true);
    }
}

//****************************************************
// Connect Springs:
//      - Connects all of the springs in the cloths
//        performing tests near the edge where certain
//        springs wouldn't have anything to connect to.
//****************************************************
void Cloth::connectSprings() {
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            Vertex* vert = this->getVertex(i, j);
            
            // Connections to the Left:
            if(i >= 2) {
                vert->connectBend(this->getVertex(i-2, j), 0);
                vert->connectStretch(this->getVertex(i-1, j), 0);
            } else {
                if(i == 1) {
                    vert->connectStretch(this->getVertex(i-1, j), 0);
                }
            }   

            // Connections Upwards:
            if(j >= 2) {
                vert->connectBend(this->getVertex(i, j-2), 1);
                vert->connectStretch(this->getVertex(i, j-1), 1);
            } else {
                if(j == 1) {
                    vert->connectStretch(this->getVertex(i, j-1), 1);
                }
            }

            // Connections to the Right
            if(i <= width - 3) {
                vert->connectBend(this->getVertex(i+2, j), 2);
                vert->connectStretch(this->getVertex(i+1, j), 2);
            } else {
                if(i == width - 2) {
                    vert->connectStretch(this->getVertex(i+1, j), 2);
                }
            }

            // Connections Downwards:
            if(j <= height - 3) {
                vert->connectBend(this->getVertex(i, j+2), 3);
                vert->connectStretch(this->getVertex(i, j+1), 3);   
            } else {
                if(j == height - 2) {
                    vert->connectStretch(this->getVertex(i, j+1), 3);
                }
            }

            // Diagonal Conections:
            
            // Connection Up-Left
            if(i >= 1 && j >= 1) {
                vert->connectShear(this->getVertex(i-1, j-1), 0);
            }

            // Conection Up-Right
            if(i <= (width - 2) && j >= 1) {
                vert->connectShear(this->getVertex(i+1, j-1), 1);
            }

            // Connection Down-Right:
            if(i <= (width - 2) && j <= (height - 2)) {
                vert->connectShear(this->getVertex(i+1, j+1), 2);
            }

            // Connection Down-Left:
            if(i >= 1 && j <= (height - 2)) {
                vert->connectShear(this->getVertex(i-1, j+1), 3);
            }

        }
    }
}

