#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include "Vertex.h"
#include "Shape.h"
#include "Spring.h"

//****************************************************
// Cloth Header Definition
//****************************************************

class Cloth {
  private:
    int width;   // Number of Vertices
    int height;  // Number of Vertices

    float actualWidth;
    float actualHeight;

    // Cloth has Set Mass and Each vertex is a portion of that
    float mass;

    float pointDrawSize;
    
    // 2D Array of Vertices: vertexMatrix[i][j] == vertexMatrix(i*w + j)
    std::vector<Vertex*> vertexMatrix;

    // TODO: Other Variables
    //      bool tearable
    //      Spring Constants
    //      Dampening Constants

    std::vector<Spring*> stretchMatrix;
    std::vector<Spring*> shearMatrix;
    std::vector<Spring*> bendMatrix;

    // Stats to Display
    int numStretchSprings;
    int numShearSprings;
    int numBendSprings;
    int numVertices;

    
    bool areNormalsUpdated;



    // Integration Type
    bool euler;

    // Private Functions and Constructor Helpers:
    void createDefaultCloth(int w, int h);
    void createVertices(glm::vec3 upLeft, glm::vec3 vertStep, glm::vec3 horizStep); // height and width already instantiated

    // Spring Setup Functions
    void connectSprings();
    void connectNewSprings();

    void addStretch(int x1, int y1, int x2, int y2);
    void addShear(int x1, int y1, int x2, int y2);
    void addBend(int x1, int y1, int x2, int y2);

    // Display and Counting Info Initializers:
    void initCounts();

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
    float getPointDrawSize() { return pointDrawSize; };

    std::vector<Spring*> getStretchSprings() { return stretchMatrix; };
    std::vector<Spring*> getShearSprings() { return shearMatrix; };
    std::vector<Spring*> getBendSprings() { return bendMatrix; };

    // Width oriented vector
    Vertex* getVertex(int w, int h) { return vertexMatrix[h*width + w]; };

    // Update Cloth:
    void update(float timestep);
    void updateNormals();

    void changeDamp(float newDamp);
    void changeTolerance(float newTolerance);
    void changeSpringConstant(float newSpring);

    // Call each spring to update Vertices
    void updateSprings();
    void applyLengthConstraints();
    void applyCorrection();

    void updateCollision(Shape* s);

    // Update Acceleration due to Forces / accels
    void addConstantAccel(glm::vec3 accel);
    void addTriangleForce(glm::vec3 force);

    void addAerodynamicDrag();

    void resetAccel();  

    void setFixedCorners(bool c1, bool c2, bool c3, bool c4);


    void printStats();
};

#endif
