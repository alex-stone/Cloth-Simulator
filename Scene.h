#ifndef SCENE_H
#define SCENE_H

#include "glm/glm.hpp"
#include "Cloth.h"
#include "Shape.h"
#include "Vertex.h"

//****************************************************
// Spring Header Definition
//****************************************************

class Scene {
  private:
    const char* inputFile;
    const char* shapeFile;

    // Cloth Info


    // Shape Info
    std::vector<Shape*> shapes;
    int numShapes;

    // Integration Method
    bool euler;
    bool useFloor;

    float currentDamp;
    float currentTolerance;
    float currentSpringConstant;

  public:
    Cloth* cloth;

    // Constructors
    Scene(const char* clothFile, const char* sceneFile, bool isEuler, bool isFloor);

    // Getters:
    bool isEuler() { return euler; };

    int getNumShapes() { return numShapes; };
    Shape* getShape(int i) { return shapes[i]; };


    // Setters:
   // bool useFloor();  // If not using floor, if using floor Remove it


    // Sets up new Cloth, and new Scenes
    void resetCloth();
    void resetScene(const char* newCloth, const char* newScene, bool isEuler, bool isFloor);
  
    void changeDamp(float newDamp);
    void changeTolerance(float newTolerance);
    void changeSpringConstant(float newSpring);

    // Functions to Load New Cloth / Shapes
    void loadShapes(const char* shapeInput);
    void loadCloth(const char* input);

    // Update Collisions through each shape
    void updateCollisions();

    // Adds Floor to Shapes Vector
    void addFloor();


};
#endif
