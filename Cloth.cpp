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

    euler = true;
}

Cloth::Cloth(int w, int h) {
    this->width = w;
    this->height = h;

    euler = true;
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

    // Set Spring Rest Lengths   
    float stretchLength = glm::length(horizStep);
    float bendLength = 2 * stretchLength;
    float shearLength = sqrt(2 * stretchLength * stretchLength);

    // Iterate through and create each Vertex
    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            glm::vec3 temp = upLeft->getPos() + ((float)i * vertStep) + ((float)j * horizStep);
            
            //I*W + j indexes vertexMatrix like a 2D array vertexMatrix[i][j]; 
            vertexMatrix[i*w + j] = new Vertex(temp.x, temp.y, temp.z);
            vertexMatrix[i*w + j]->setSpringRestLengths(stretchLength, bendLength, shearLength); 
            //vertexMatrix[i*w + j]->setSpringConstants(stretchConst, shearConst, bendConst);

            vertexMatrix[i*w + j]->setPosition(j, i);
        }
    }

    this->connectSprings();

}

void Cloth::updateCollision(Shape* s) {
    for(int i = 0; i < height*width; i++) {
        s->collide(vertexMatrix[i]);
    }
}
/*

// Iterate through each Square and for each 2 triangles, calculate normals
void Cloth::updateNormals() {

    for(int i = 0; i < height*width; i++) {
        vertexMatrix[i]->resetNorm();
    }

    for(int i = 0; i < this->width - 1; i++) {
        for(int j = 0; j < this->height - 1; j++) {

            // V1 cross V2
            Vertex* a = this->getVertex(i+1, j);
            Vertex* b = this->getVertex(i, j);
            Vertex* c = this->getVertex(i, j+1);

            glm::vec3 vec1 = b->getPos() - a->getPos();
            glm::vec3 vec2 = c->getPos() - a->getPos();

            glm::vec3 triNormal = glm::cross(vec1, vec2);

            a->updateNormal(triNormal);
            b->updateNormal(triNormal);
            c->updateNormal(triNormal);

            a = this->getVertex(i+1, j+1);
            b = this->getVertex(i+1, j);
            c = this->getVertex(i, j+1);

            vec1 = b->getPos() - a->getPos();
            vec2 = c->getPos() - a->getPos();

            triNormal = glm::cross(vec1, vec2);

            a->updateNormal(triNormal);
            b->updateNormal(triNormal);
            c->updateNormal(triNormal);


        }
    }

}*/

void Cloth::updateNormals(){
    for(int i = 0; i< this->height -1; i++){
        for(int z= 0; z < this->width -1 ; z++){
            glm::vec3 triNormal =vertexMatrix[z * this->width + (i+1)]->findNormal(vertexMatrix[z * this->width + i], vertexMatrix[(z+1) * this->width + i]);
            vertexMatrix[z * this->width + (i+1)]->updateNormal(triNormal);
            vertexMatrix[z * this->width + i]->updateNormal(triNormal);
            vertexMatrix[(z+1) * this->width + i]->updateNormal(triNormal);

            glm::vec3 tempNormal = vertexMatrix[(z+1) * this->width + (i+1)]->findNormal(vertexMatrix[z * this->width + (i+1)], vertexMatrix[(z+1) * this->width + i]);
            vertexMatrix[(z+1) * this->width + (i+1)]->updateNormal(tempNormal);
            vertexMatrix[z * this->width + (i+1)]->updateNormal(tempNormal);
            vertexMatrix[(z+1) * this->width + i]->updateNormal(tempNormal);
        }
    }
    
}


void Cloth::addExtForce(glm::vec3 force){
    for(int i = 0; i< this->height -1; i++){
        for(int z= 0; z < this->width -1 ; z++){
            glm::vec3 triNormal =glm::normalize(vertexMatrix[z * this->width + (i+1)]->findNormal(vertexMatrix[z * this->width + i], vertexMatrix[(z+1) * this->width + i]));
            glm::vec3 tempNormal = glm::normalize(vertexMatrix[(z+1) * this->width + (i+1)]->findNormal(vertexMatrix[z * this->width + (i+1)], vertexMatrix[(z+1) * this->width + i]));
            triNormal *= glm::dot(triNormal, force);
            tempNormal *= glm::dot(tempNormal, force);
            vertexMatrix[z * this->width + (i+1)]->updateAccel(triNormal);
            vertexMatrix[z * this->width + (i)]->updateAccel(triNormal);
            vertexMatrix[(z+1) * this->width + (i)]->updateAccel(triNormal);
            vertexMatrix[(z+1) * this->width + (i+1)]->updateAccel(tempNormal);
            vertexMatrix[(z) * this->width + (i+1)]->updateAccel(tempNormal);
            vertexMatrix[(z+1) * this->width + (i)]->updateAccel(tempNormal);

            }
     }
}






void Cloth::update(float timestep) {
    // Iterate through vertexMatrix, and update each individual particle

    for(int i = 0; i < height*width; i++) {
        vertexMatrix[i]->update(timestep, euler);
    }

}

/*void Cloth::update(float timestep,glm::vec3 spherePos, float sphereRadius) {
    // Iterate through vertexMatrix, and update each individual particle

    for(int i = 0; i < height*width; i++) {
        vertexMatrix[i]->updateCollisions(spherePos,sphereRadius);
        vertexMatrix[i]->update(timestep,euler);
        
    }

}
*/

void Cloth::addExternalForce(glm::vec3 externalForce) {
    
    for(int i = 0; i < height*width; i++) {
        vertexMatrix[i]->updateAccel(externalForce);
    }

}

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

