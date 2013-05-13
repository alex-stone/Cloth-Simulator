#include <iostream>
#include <math.h>
#include <vector>
#include "glm/glm.hpp"

#include "Cloth.h"
#include "Vertex.h"


//****************************************************
// Cloth Class - Constants
//****************************************************
const float UNIT_STRETCH = 100.0f;
const float UNIT_SHEAR = 100.0f;
const float UNIT_BEND = 50.0f;

// Debug Variables
bool debug = false;

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

    if(debug) {
        std::cout << "This Cloth is of dimensions Width: " << this->width << " by Height: " << this->height << std::endl;
    }


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
    
    // Sets size of the Vector holding the vertices to W * H
    vertexMatrix.resize(this->width * this->height);

    // Set Spring Rest Lengths   
    float stretchLength = glm::length(horizStep);
    float bendLength = 2 * stretchLength;
    float shearLength = sqrt(2 * stretchLength * stretchLength);

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

            // Get Vertex must be called (width, height);
            Vertex* v1 = getVertex(w, h);
            Vertex* v2 = getVertex(w, h+1);
            Vertex* v3 = getVertex(w+1, h);
            Vertex* v4 = getVertex(w+1, h+1);

            glm::vec3 triNormal1 = glm::normalize(v1->findNormal(v2,v3));

            v1->updateNormal(triNormal1);
            v2->updateNormal(triNormal1);
            v3->updateNormal(triNormal1);

            glm::vec3 triNormal2 = glm::normalize(v4->findNormal(v3,v2));
            
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

            Vertex* v1 = getVertex(w,h);
            Vertex* v2 = getVertex(w, h+1);
            Vertex* v3 = getVertex(w+1, h);
            Vertex* v4 = getVertex(w+1, h+1);            

            // Calculates Force Contribution on the first Triangle
            glm::vec3 triNormal1 = glm::normalize(v1->findNormal(v2,v3));
            glm::vec3 triForce1 = triNormal1 * glm::dot(triNormal1, force);

            v1->addForce(triForce1);
            v2->addForce(triForce1);
            v3->addForce(triForce1);

            // Calculates Force Contribution on the Second Triangle
            glm::vec3 triNormal2 = glm::normalize(v4->findNormal(v3,v2));
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
void Cloth::connectNewSprings() {
    for(int h = 0; h < this->height, h++) {
        for(int w = 0; w < this->width; w++) {

            // Only connect Springs To the Right & Downwards to avoid duplicates

            // Connections to the Right
            if(w < width - 1) {                    // If there is a vertex 1 spaces to the right
                this->addStretch(w, h, w+1, h);

                
                if(w < width - 2) {                // If there is a vertex 2 spaces to the right
                    this->addBend(w, h, w+2, h);
                }
            }

            // Connect Downwards
            if(h < height - 1) {
                addStretch(w, h, w, h+1);

                // If There are vertices 2 spaces down
                if(h < height - 2) {
                    addBend(w, h, w, h+2);
                }
            }
            
            // Add Down Right Shear
            if(w < (width - 1) && h < (height - 1)) {
                addShear(w, h, w+1, h+1);
            }

            // Add Down-Left Shear
            if (w >= 1 && h < (height - 1)) {
                addShear(w, h, w-1, h+1);
            }
        }
    }
}

//****************************************************
// Connect Springs:
//      - Connects all of the springs in the cloths
//        performing tests near the edge where certain
//        springs wouldn't have anything to connect to.
//****************************************************
void Cloth::connectSprings() {
    for(int h = 0; h < height; h++) {
        for(int w = 0; w < width; w++) {
            Vertex* vert = this->getVertex(w, h);

            // Connections to the Left:
            if(w >= 2) {
                vert->connectBend(this->getVertex(w-2, h), 0);
                vert->connectStretch(this->getVertex(w-1, h), 0);
            } else {
                if(w == 1) {
                    vert->connectStretch(this->getVertex(w-1, h), 0);
                }
            }   

            // Connections Upwards:
            if(h >= 2) {
                vert->connectBend(this->getVertex(w, h-2), 1);
                vert->connectStretch(this->getVertex(w, h-1), 1);
            } else {
                if(h == 1) {
                    vert->connectStretch(this->getVertex(w, h-1), 1);
                }
            }


            // Connections to the Right
            if(w <= width - 3) {
                vert->connectBend(this->getVertex(w+2, h), 2);
                vert->connectStretch(this->getVertex(w+1, h), 2);
            } else {
                if(w == width - 2) {
                    vert->connectStretch(this->getVertex(w+1, h), 2);
                }
            }

            // Connections Downwards:
            if(h <= height - 3) {
                vert->connectBend(this->getVertex(w, h+2), 3);
                vert->connectStretch(this->getVertex(w, h+1), 3);   
            } else {
                if(h == height - 2) {
                    vert->connectStretch(this->getVertex(w, h+1), 3);
                }
            }

            // Diagonal Conections:
            
            // Connection Up-Left
            if(w >= 1 && h >= 1) {
                vert->connectShear(this->getVertex(w-1, h-1), 0);
            }

            // Conection Up-Right
            if(w <= (width - 2) && h >= 1) {
                vert->connectShear(this->getVertex(w+1, h-1), 1);
            }

            // Connection Down-Right:
            if(w <= (width - 2) && h <= (height - 2)) {
                vert->connectShear(this->getVertex(w+1, h+1), 2);
            }

            // Connection Down-Left:
            if(w >= 1 && h <= (height - 2)) {
                vert->connectShear(this->getVertex(w-1, h+1), 3);
            }
        }
    }
}

//****************************************************
// Add Stretch
//      - Adds a stretch spring to stretchMatrix
//****************************************************
void addStretch(int x1, int y1, int x2, int y2) {
    Vertex* v1 = this->getVertex(x1, y1);
    Vertex* v2 = this->getVertex(x2, y2);
    Spring* temp = new Spring(v1, v2, "STRETCH");
    stretchMatrix.push_back(temp);
}
//****************************************************
// Add Shear
//      - Adds a shear spring to shearMatrix
//****************************************************
void addShear(int x1, int y1, int x2, int y2) {
    Vertex* v1 = this->getVertex(x1, y1);
    Vertex* v2 = this->getVertex(x2, y2);
    Spring* temp = new Spring(v1, v2, "SHEAR");
    shearMatrix.push_back(temp);
}
//****************************************************
// Add Bend
//      - Adds a Bend spring to bendMatrix
//****************************************************
void addBend(int x1, int y1, int x2, int y2) {
    Vertex* v1 = this->getVertex(x1, y1);
    Vertex* v2 = this->getVertex(x2, y2);
    Spring* temp = new Spring(v1, v2, "BEND");
    bendMatrix.push_back(temp);   
}

