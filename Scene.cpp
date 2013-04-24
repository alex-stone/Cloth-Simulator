#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include "glm/glm.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else     
#include <GL/glut.h>
#include <GL/glu.h> 
#endif
             
#include <time.h>
#include <math.h>
#include "Cloth.h"
#include "Vertex.h"
#include "Spring.h"

#define PI 3.14159265

using namespace std;

class Viewport {
  public:
    int w, h; // width and height
};

//****************************************************
// Global Variables 
//****************************************************
Viewport    viewport;
Cloth*      cloth;

// OpenGL Drawing Variables
bool wire;
bool smooth;


//****************************************************
// Glut Functions
//****************************************************

void initScene() {

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );

    // Initialize Viewport Size
    viewport.w = 400;
    viewport.h = 400;

    // The Size and Position of the Window
    glutInitWindowSize(viewport.w, viewport.h);
    glutInitWindowPosition(0,0);
    glutCreateWindow("CS184 - Final Project");


}

void myReshape(int w, int h) {
    viewport.w = w;
    viewport.h = h;

    float aspectRatio = (float) w / (float) h;

    glViewport(0, 0, viewport.w, viewport.h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45, aspectRatio, 1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    // Set Camera
    
    glBegin(GL_QUAD_STRIP);

    // Iterate through each vertex in the cloth;
    for(int i = 0; i < cloth->getWidth(); i++) {
        for(int j = 0; j < cloth->getHeight()-1; j++) {
            Vertex* temp = cloth->getVertex(i, j); 
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);
            
            temp = cloth->getVertex(i, j+1);
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);
        }
    }
    
    glEnd();

    glFlush();
    glutSwapBuffers();
}

void keyPress(unsigned char key, int x, int y) {

}

//****************************************************
// LoadCloth  Function
//      - Reads in Cloth Information from file
//      - First line Width Height
//      - Next 4 lines four corners of cloth
//****************************************************
void loadCloth(char* input) {
    std::ifstream inpfile(input, ifstream::in);
    
    int width, height;

    Vertex* corners[4];

    if(inpfile.good()) {
        inpfile >> width;
        inpfile >> height; 

        
        for(int i = 0; i < 4; i++) {
            float x,y,z;
            inpfile >> x;
            inpfile >> y;
            inpfile >> z;

            corners[i] = new Vertex(x,y,z);

        }

    }
    
    inpfile.close();

    cloth = new Cloth(width, height, corners[0], corners[1], corners[2], corners[3]);

   
}

int main(int argc, char *argv[]) {
   
    // Process Inputs
    
    // Load Cloth
    if(argc == 2) {
        loadCloth(argv[1]);
    } else {
        char* inputFile = "test/cloth.test";
        loadCloth(inputFile);
    }

    // Initialize GLUT
    glutInit(&argc, argv);

    initScene();

    // GLUT Loop    
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(keyPress);
    glutMainLoop();

}




