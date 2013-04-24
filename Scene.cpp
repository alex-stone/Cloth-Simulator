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
bool paused;
bool light;

// OpenGL Perspective Variables & Constants:
GLdouble aspectRatio;
const GLdouble FOV_Y = 45.0;
const GLdouble Z_NEAR = 1.0;
const GLdouble Z_FAR = 40.0;

// OpenGL Transformation Variables:
GLfloat theta;
GLfloat phi;
GLfloat xTranslate;
GLfloat yTranslate;
GLfloat zTranslate;

// OpenGL Transformation Constants;
const GLfloat TRANSLATE_INC = 0.05f;
const GLfloat Z_TRANSLATE_INC = 0.2f;
const GLfloat ROTATE_INC = 3.0f;

// Animation Variables:
int step;

// Want a key to step through the Animation



//****************************************************
// Glut Functions
//****************************************************

void initScene() {

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

    // Initialize Viewport Size
    viewport.w = 400;
    viewport.h = 400;

    // The Size and Position of the Window
    glutInitWindowSize(viewport.w, viewport.h);
    glutInitWindowPosition(0,0);
    glutCreateWindow("CS184 - Final Project");

    // Initialize Camera Properties:
    theta = 0.0f;
    phi = 0.0f;
    xTranslate = 0.0f;
    yTranslate = 0.0f;
    zTranslate = -10.0f;

    // Initialize Drawing Variables:
    wire = true;
    smooth = false;
    light = false;

    // Initializes Wireframe to be ON 
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glShadeModel(GL_FLAT);

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);

    glDepthFunc(GL_LEQUAL);
    
}

//****************************************************
// MyReshape 
//     - reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
    viewport.w = w;
    viewport.h = h;

    aspectRatio = ((GLdouble) w / (GLdouble) h);

    glViewport(0, 0, viewport.w, viewport.h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(FOV_Y, aspectRatio, Z_NEAR, Z_FAR);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void printHUD(float x, float y, float r, float g, float b, const char* text) {
    glColor3f(r,g,b);
    glRasterPos2f(x,y);
    while(*text) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *text);
        text++;
    }

}

void drawTestLine() {
    glBegin(GL_LINES);
    
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
    
    
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
   
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
    
    glEnd();

}

//****************************************************
// RenderCloth
//      - Render's the Cloth
//****************************************************
void renderCloth() {

    for(int h = 0; h < cloth->getHeight()-1; h++) {

        glBegin(GL_QUAD_STRIP);

        for(int w = 0; w < cloth->getWidth(); w++) {

            glColor3f(1.0f, 1.0f, 1.0f);

            Vertex* temp = cloth->getVertex(w, h+1 );
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);

            temp = cloth->getVertex(w, h);
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);

        }

        glEnd();
    }
}

//****************************************************
// MyDisplay 
//     - reshape viewport if the window is resized
//****************************************************
void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Zeroe's Out 
    glLoadIdentity();

    // Set Camera
    glTranslatef(xTranslate, yTranslate, zTranslate);
   
    glRotatef(theta, 1.0f, 0.0f, 0.0f);
    glRotatef(phi, 0.0f, 0.0f, 1.0f);

    // Iterate through each vertex in the cloth;
   
    drawTestLine();
    
    //renderCloth(); 
/*
    for(int h = 0; h < cloth->getHeight()-1; h++) {
    
        glBegin(GL_QUAD_STRIP);
 
        for(int w = 0; w < cloth->getWidth(); w++) {
   
            glColor3f(1.0f, 1.0f, 1.0f);
    
            Vertex* temp = cloth->getVertex(w, h+1 );
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);
          
            temp = cloth->getVertex(w, h);
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);

            
        }
        glEnd();
    }
*/
    // Setup For 2D:

/*
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity;

    glOrtho(0, viewport.w, 0, viewport.h, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    const char* ThetaOutput = "Theta = ";
    printHUD(-2,-2 , 1.0f, 1.0f, 1.0f, ThetaOutput);
*/
    glFlush();
    glutSwapBuffers();
}

//****************************************************
// On keyPress:
//   if key:
//      's':  Toggle Flat & Smooth Shading
//      'w':  Toggle Filled & Wire Framing
//      '+':  Zoom In
//      '-':  Zoom out
//      ' ':  Close the window
//****************************************************
void keyPress(unsigned char key, int x, int y) {
    
    switch(key) {
        case 'w':
            wire = !wire;
            if(wire) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            break;
        case '+':
            zTranslate += Z_TRANSLATE_INC;
            break;
        case '-':
            zTranslate -= Z_TRANSLATE_INC;
            break;
        case ' ':
            std::exit(1);
            break;

    }

    myDisplay();
}

//****************************************************
// On Arrow Key Press:
//   
//****************************************************
void arrowKeyPress(int key, int x, int y) {
    bool shift = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);

    switch(key) {
        case 100:   // Left Arrow
            if(shift) {
                xTranslate -= TRANSLATE_INC;
            } else {
                phi -= ROTATE_INC;
            }
            break;
        case 101:   // Up Arrow
            if(shift) {
                yTranslate += TRANSLATE_INC;
            } else {
                theta -= ROTATE_INC;
            }
            break;
        case 102:   // Right Arrow
            if(shift) {
                xTranslate += TRANSLATE_INC;
            } else {
                phi += ROTATE_INC;
            }
            break;
        case 103:   // Down Arrow
            if(shift) {
                yTranslate -= TRANSLATE_INC;
            } else {
                theta += ROTATE_INC;
            }
            break;
    }

    myDisplay();
}


//****************************************************
// LoadCloth  Function
//      - Reads in Cloth Information from file
//      - First line Width Height
//      - Next 4 lines four corners of cloth
//****************************************************
void loadCloth(const char* input) {
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
        const char* inputFile = "test/cloth.test"; 
        loadCloth(inputFile);
    }

    // Initialize GLUT
    glutInit(&argc, argv);

    initScene();

    // GLUT Loop    
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(keyPress);
    glutSpecialFunc(arrowKeyPress);
    glutMainLoop();

    return 0;
}




