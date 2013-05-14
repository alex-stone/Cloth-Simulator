#include <iostream>
#include <math.h>
#include <vector>
#include "glm/glm.hpp"

#include "Cloth.h"
#include "Vertex.h"


//****************************************************
// Cloth Class - Constants
//****************************************************

// Debug Variables
bool debug = false;
bool useSpringForce = false;

//****************************************************
// Cloth Class - Constructors
//****************************************************
Cloth::Cloth() {
    euler = true;
    initCounts();

    createDefaultCloth(20, 20);
}

Cloth::Cloth(int w, int h) {
    euler = true;
    initCounts();

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
    initCounts();

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


    mass = 100.0f;

    // Create the vertices and Connect all the Springs
    createVertices(upLeft->getPos(), vertVec, horizVec);
    

    //connectSprings();

    connectNewSprings();

}


//****************************************************
// Cloth Constructor called from Parser
//          - Takes the 4 corners, and divides Cloth
//            Into W x H vertices
//
//****************************************************
Cloth::Cloth(int w, int h, Vertex* upLeft, Vertex* upRight, Vertex* downRight, Vertex* downLeft, bool isEuler) {
    initCounts();

    this->width = w;
    this->height = h;

    euler = isEuler;

    glm::vec3 vertVec = upLeft->vectorTo(downLeft);
    glm::vec3 horizVec = upLeft->vectorTo(upRight);

    glm::vec3 vertStep = vertVec / (float)(h-1);
    glm::vec3 horizStep = horizVec / (float)(w-1);

    mass = 100.0f;
    createVertices(upLeft->getPos(), vertStep, horizStep);
    
    connectNewSprings();
}

//****************************************************
// Cloth Class - Constuctor Helpers
//****************************************************
void Cloth::initCounts() {
    numVertices = 0;
    numStretchSprings = 0;
    numShearSprings = 0;
    numBendSprings = 0;

    areNormalsUpdated = false;
}

void Cloth::changeDamp(float newDamp) {
    for(int i = 0; i < width*height; i++) {
        vertexMatrix[i]->setDamp(newDamp);
    }
}

void Cloth::changeTolerance(float newTolerance) {
    for(int i = 0; i < stretchMatrix.size(); i++) {
        stretchMatrix[i]->setTolerance(newTolerance);
    }

    for(int j = 0; j < shearMatrix.size(); j++) {
        shearMatrix[j]->setTolerance(newTolerance);
    }

    for(int k = 0; k < bendMatrix.size(); k++) {
        bendMatrix[k]->setTolerance(newTolerance);
    }
}

void Cloth::changeSpringConstant(float newSpring) {
    for(int i = 0; i < stretchMatrix.size(); i++) {
        stretchMatrix[i]->setSpringConstant(newSpring);
    }

    for(int j = 0; j < shearMatrix.size(); j++) {
        shearMatrix[j]->setSpringConstant(newSpring);
    }

    for(int k = 0; k < bendMatrix.size(); k++) {
        bendMatrix[k]->setSpringConstant(newSpring);
    }

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

    mass = 100.0f;
    createVertices(upLeft, vertStep, horizStep);
    
    //connectSprings();
    
    connectNewSprings();
}

//****************************************************
// Create Vertices:
//      - Sets up the vertices, based on the top left
//          point, and the vector that goes to the
//          next vertex, both vertically & horizontally
//      - Width & Height already determined
//****************************************************
void Cloth::createVertices(glm::vec3 upLeft, glm::vec3 vertStep, glm::vec3 horizStep) {
    
    // Sets the radius of the spheres that will be drawn for the points when drawing the structure
    pointDrawSize = glm::length(horizStep)*0.2;
    std::cout << "Poind Draw Size = " << pointDrawSize << std::endl;

    // Sets size of the Vector holding the vertices to W * H
    numVertices = this->width * this->height;
    vertexMatrix.resize(numVertices);

    mass = 100.0f;

    float vertexMass = mass / (float) numVertices;
    std::cout << "Vertex Mass = " << vertexMass << std::endl;

    actualWidth = glm::length(horizStep) * (this->width - 1); 
    actualHeight = glm::length(vertStep) * (this->height - 1);

    // Iterate through and create each Vertex
    for(int h = 0; h < this->height; h++) {
        for(int w = 0; w < this->width; w++) {

            //I*W + j indexes vertexMatrix like a 2D array vertexMatrix[i][j]; 
            int vertIndex = h * (this->width) + w;

            glm::vec3 temp = upLeft + ((float)h * vertStep) + ((float)w * horizStep);
            
            vertexMatrix[vertIndex] = new Vertex(temp.x, temp.y, temp.z);

            // Sets its position in the Grid of the cloth
            vertexMatrix[vertIndex]->setPosition(w, h);
            vertexMatrix[vertIndex]->setMass(vertexMass);

        }        
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

    updateSprings();
    //addAerodynamicDrag();

    for(int i = 0; i < height*width; i++) {
        vertexMatrix[i]->update(timestep, euler);
    }

    if(useSpringForce) {
        //. applyCorrection();
        applyLengthConstraints();
    }

    areNormalsUpdated = false;
}


void Cloth::applyCorrection() {
    for(int i = 0; i < stretchMatrix.size(); i++) {
        stretchMatrix[i]->applyCorrection();
    }

    for(int j = 0; j < shearMatrix.size(); j++) {
        shearMatrix[j]->applyCorrection();
    }

    for(int k = 0; k < bendMatrix.size(); k++) {
        bendMatrix[k]->applyCorrection();
    }
}

//****************************************************
// Update Springs:
//      - Goes through Spring and applies the force
//        to Vertices its connected to
//****************************************************
void Cloth::updateSprings() {
    if(useSpringForce) {

        for(int i = 0; i < stretchMatrix.size(); i++) {
            stretchMatrix[i]->applyForce();
        }

        for(int j = 0; j < shearMatrix.size(); j++) {
            shearMatrix[j]->applyForce();
        }

        for(int k = 0; k < bendMatrix.size(); k++) {
            bendMatrix[k]->applyForce();
        }

    } else {
        applyCorrection();
    }

}

void Cloth::applyLengthConstraints() {
    for(int i = 0; i < stretchMatrix.size(); i++) {
        stretchMatrix[i]->lengthConstraint();
    }

    for(int j = 0; j < shearMatrix.size(); j++) {
        shearMatrix[j]->lengthConstraint();
    }

    for(int k = 0; k < bendMatrix.size(); k++) {
        bendMatrix[k]->lengthConstraint();
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

    // Only Update if they are not updated
    if(true) {

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

        areNormalsUpdated = true;

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

void calcDragOnTriangle(Vertex* v1, Vertex* v2, Vertex* v3) {

    // Velocity is Triangle Velocity - Air Velocity

    glm::vec3 avgVel = (v1->getVelocity() + v2->getVelocity() + v3->getVelocity())/(3.0f);
   

    std::cout << "Avg Velocity = " << avgVel.x << ", " << avgVel.y << ", " << avgVel.z << ")" << std::endl;


    glm::vec3 cross = glm::cross(v1->vectorTo(v2), v1->vectorTo(v3));

    // To Do: Test if this is correct

    float rho = 1.1f;
    float dragCoeff = 1.0f;

    glm::vec3 factor = cross * (glm::length(avgVel) * glm::dot(avgVel, cross) / (2 * glm::length(cross)));



    std::cout << "FACTOR LENGTH = " << glm::length(factor) << std::endl;

    glm::vec3 force = (0.5f) * rho * dragCoeff * factor;

    v1->addForce(force/3.0f);
    v2->addForce(force /3.0f);
    v3->addForce(force/3.0f);

    std::cout << "Aerodynamic Force = (" << force.x << ", " << force.y << ", " << force.z << ")" << std::endl;

}

//****************************************************
// Add Aerodynamic Drag
//****************************************************
void Cloth::addAerodynamicDrag() {

    for(int h = 0; h < this->height - 1; h++) {
        for(int w = 0; w < this->width - 1; w++) {
            Vertex* v1 = getVertex(w,h);
            Vertex* v2 = getVertex(w, h+1);
            Vertex* v3 = getVertex(w+1, h);
            Vertex* v4 = getVertex(w+1, h+1);

            calcDragOnTriangle(v1, v2, v3);

            calcDragOnTriangle(v4, v3, v2);


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
//      - Connects all of the Springs in the Cloth
//      - Only Checks % adds Springs downwards and
//          leftwards, to avoid duplicates
//****************************************************
void Cloth::connectNewSprings() {
    for(int h = 0; h < this->height; h++) {
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
            if(h < height - 1) {                // If There are vertices 1 spaces down
                addStretch(w, h, w, h+1);

                if(h < height - 2) {            // If There are vertices 2 spaces down
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
// Add Stretch
//      - Adds a stretch spring to stretchMatrix
//****************************************************
void Cloth::addStretch(int x1, int y1, int x2, int y2) {
    Vertex* v1 = this->getVertex(x1, y1);
    Vertex* v2 = this->getVertex(x2, y2);

    Spring* temp = new Spring(v1, v2, "STRETCH");
    stretchMatrix.push_back(temp);
    numStretchSprings++;
}
//****************************************************
// Add Shear
//      - Adds a shear spring to shearMatrix
//****************************************************
void Cloth::addShear(int x1, int y1, int x2, int y2) {
    Vertex* v1 = this->getVertex(x1, y1);
    Vertex* v2 = this->getVertex(x2, y2);

    Spring* temp = new Spring(v1, v2, "SHEAR");
    shearMatrix.push_back(temp);
    numShearSprings++;
}
//****************************************************
// Add Bend
//      - Adds a Bend spring to bendMatrix
//****************************************************
void Cloth::addBend(int x1, int y1, int x2, int y2) {
    Vertex* v1 = this->getVertex(x1, y1);
    Vertex* v2 = this->getVertex(x2, y2);

    Spring* temp = new Spring(v1, v2, "BEND");
    bendMatrix.push_back(temp);   
    numBendSprings++;
}

//****************************************************
// Print Stats
//      - Prints Cloth Stats:
//****************************************************
void Cloth::printStats() {
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << " Cloth Information: " << std::endl;
    std::cout << "---------------------------------------" << std::endl;    
    std::cout << "Actual Dimensions: " << actualWidth << " x " << actualHeight << std::endl;
    std::cout << "Vertex Dimensions: " << width << " x " << height << std::endl;
    std::cout << "Total # of Vertices: " << numVertices << std::endl;
    std::cout << "Total # of Springs: " << numStretchSprings + numShearSprings + numBendSprings << std::endl;
    std::cout << "# of STRETCH: " << numStretchSprings << std::endl;
    std::cout << "# of   SHEAR: " << numShearSprings << std::endl;
    std::cout << "# of    BEND: " << numBendSprings << std::endl;

    Spring::printStats();

    std::cout << "---------------------------------------" << std::endl;   
    std::cout << "---------------------------------------" << std::endl;           

}



//****************************************************
// Connect Springs:
//      - Connects all of the springs in the cloths
//        performing tests near the edge where certain
//        springs wouldn't have anything to connect to.
//****************************************************

/*

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

*/
