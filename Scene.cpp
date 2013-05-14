#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
             
#include <time.h>
#include <math.h>

#include "glm/glm.hpp"
#include "Scene.h"
#include "Cloth.h"
#include "Vertex.h"
#include "Shape.h"
#include "Sphere.h"
#include "Plane.h"

using namespace std;

//****************************************************
// Global Variables
//****************************************************
bool debugStats = true;
float DEF_DAMP = 0.015f;
float DEF_TOL = 0.01f;
float DEF_SPRING = 100.0f;

//****************************************************
// Scene Constructor:
//****************************************************
Scene::Scene(const char* clothFile, const char* sceneFile, bool isEuler, bool isFloor) {
    resetScene(clothFile, sceneFile, isEuler, isFloor);

    changeDamp(DEF_DAMP);
    changeTolerance(DEF_TOL);
    changeSpringConstant(DEF_SPRING);
}

void Scene::resetCloth() {
    this->cloth = NULL;
    this->loadCloth(inputFile);

}

void Scene::changeDamp(float newDamp) {
    currentDamp = newDamp;

    cloth->changeDamp(newDamp);
}

void Scene::changeTolerance(float newTolerance) {
    currentTolerance = newTolerance;

    cloth->changeTolerance(newTolerance);
}

void Scene::changeSpringConstant(float newSpring) {
    currentSpringConstant = newSpring;

    cloth->changeSpringConstant(newSpring);

}

//****************************************************
// Reset Scene Function:
//****************************************************
void Scene::resetScene(const char* newCloth, const char* newScene, bool isEuler, bool isFloor) {
    
    useFloor = isFloor;

    euler = isEuler;

    // Reset the Cloth with a new Cloth
    if(newCloth != inputFile) {
        this->inputFile = newCloth;

        this->cloth = NULL;
        this->loadCloth(newCloth);
 
    }

    // Reset the Scene with new Scene
    if(newScene != shapeFile) {
        this->shapeFile = newScene;

        // Reset Scene Variables
        this->shapes.clear();
        numShapes = 0;

        this->loadShapes(newScene);

        if(useFloor) {
             this->addFloor();
        }
    }
}

//****************************************************
// Update Collisions:
//      - Iterates through each Shape and tests the
//      Cloth for collisions
//****************************************************
void Scene::updateCollisions() {
    for(int i = 0; i < shapes.size(); i++) {
        this->cloth->updateCollision(shapes[i]);
    }
}


//****************************************************
// Add Floor
//      - Adds Floor to the Shapes List
//****************************************************
void Scene::addFloor() {

    numShapes++;

    Shape* s;

    float width = 5.0f;
    float height = -2.0f;

    glm::vec3 topLeft(-width, height, -width);
    glm::vec3 topRight(width, height, -width);
    glm::vec3 lowRight(width, height, width);
    glm::vec3 lowLeft(-width, height, width);

    s = new Plane(topLeft, topRight, lowRight, lowLeft);
    s->setFloor();

    shapes.push_back(s);
}


//****************************************************
// LoadShapes Function
//      - Creates the draw list for all the shapes
//****************************************************
void Scene::loadShapes(const char* shapeInput) {

    std::ifstream inpfile(shapeInput, std::ifstream::in);

    if(inpfile.good()){
        inpfile >> numShapes;
        std::string type;
    
        for(int i = 0; i < numShapes; i++) {

            inpfile >> type;
            Shape* s;
        
            if(string(type) == "sphere") {
                float x,y,z,r;
                inpfile >> x;
                inpfile >> y;
                inpfile >> z;
                inpfile >> r;

                glm::vec3 center(x,y,z);

                s = new Sphere(center, r); 
            }

            if(string(type) == "plane") {
                float x1, y1, z1;
                float x2, y2, z2;
                float x3, y3, z3;
                float x4, y4, z4;

                inpfile >> x1;
                inpfile >> y1;
                inpfile >> z1;

                inpfile >> x2;
                inpfile >> y2;
                inpfile >> z2;

                inpfile >> x3;
                inpfile >> y3;
                inpfile >> z3;

                inpfile >> x4;
                inpfile >> y4;
                inpfile >> z4;

                glm::vec3 topLeft(x1, y1, z1);
                glm::vec3 topRight(x2, y2, z2);
                glm::vec3 lowRight(x3, y3, z3);
                glm::vec3 lowLeft(x4, y4, z4);
            
                s = new Plane(topLeft, topRight, lowRight, lowLeft);    
            }
            
            shapes.push_back(s);
        }
    }
}

//****************************************************
// LoadCloth  Function
//      - Reads in Cloth Information from file
//      - First line Width Height
//      - Next 4 lines four corners of cloth
//****************************************************
void Scene::loadCloth(const char* input) {
    std::ifstream inpfile(input, std::ifstream::in);
    

    //int width, height;
    int density;
    bool c1, c2, c3, c4;
    bool euler;

    Vertex* corners[4];

    if(inpfile.good()) {
        //inpfile >> width;
        //inpfile >> height; 
        
        inpfile >> density;

        for(int i = 0; i < 4; i++) {
            float x,y,z;
            inpfile >> x;
            inpfile >> y;
            inpfile >> z;

            corners[i] = new Vertex(x,y,z);
        }

        std::string temp;
    
        inpfile >> temp;
        c1 = (temp == "true");
        inpfile >> temp;
        c2 = (temp == "true");
        inpfile >> temp;
        c3 = (temp == "true");
        inpfile >> temp;
        c4 = (temp == "true");
    }
    
    inpfile.close();
    

    cloth = NULL;

    cloth = new Cloth(density, corners[0], corners[1], corners[2], corners[3], euler);

    if(debugStats) {
        cloth->printStats();
    }

    //cloth = new Cloth(width, height, corners[0], corners[1], corners[2], corners[3], euler);
    cloth->setFixedCorners(c1, c2, c3, c4);

}

/*
int main(int argc, char *argv[]) {
   
    // Initialize GLUT
    glutInit(&argc, argv);
    
    // Process Inputs & sets variables: inputFule, shapeFile, euler
    processInputs(argc, argv);

    // Loads Cloth & Shapes Info
    //loadCloth(inputFile);
    //loadShapes(shapeFile);

    scene = new Scene(inputFile, shapeFile, euler);

    initScene();

    if(useFloor) {
         scene->addFloor();
    }

    // Initializes Window & OpenGL Settings
    

    // Initialize Shape Draw Lists
    shapeDrawLists = new GLuint[numShapes]; 
    makeDrawLists();
    
    // GLUT Loop    
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(keyPress);
    glutSpecialFunc(arrowKeyPress);
    glutMainLoop();

    return 0;
}*/



