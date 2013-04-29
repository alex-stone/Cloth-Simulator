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
const char* inputFile;
<<<<<<< HEAD
=======
glm::vec3 spherePos(0.0f,-4.0f,-2.0f);
float sphereRadius = 2.0f;
>>>>>>> Collisions updated

// OpenGL Drawing Variables
bool wire;
bool smooth;
bool running;   // Is simulation running in real-time or paused for step through
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
float timestep = 0;
float oldTime = 0;

int numTimeSteps = 30;
const float STEP = 0.01f;

// Position Update Method Variables: Command Lines
bool euler;

// Forces:
bool gravity;
glm::vec3 gravityForce(0.0f, -1.0f, 0.0f);


// Debug Variables:
bool debugFunc = false;

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
    running = false;
    
    gravity = true;

    // Set up Lights:
    GLfloat Specular[] = {0.0f, 0.2f, 0.8f};
    GLfloat Ambient[] = {0.0f, 0.2f, 0.7f};
    GLfloat Diffuse[] = {0.2f, 0.3f, 0.6f};
    GLfloat LightOnePos[] = {-2.0f, 2.0f, 2.0f};

    glLightfv(GL_LIGHT1, GL_SPECULAR, Specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, Ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, Diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, LightOnePos);

    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);

    // Initializes Wireframe to be ON 
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glShadeModel(GL_FLAT);

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);

    glDepthFunc(GL_LEQUAL);
    
}
<<<<<<< HEAD

//****************************************************
// GLUT 3D Setup
//     - Sets all the openGL/GLUT Modes and views for
//      Rendering 3D Objects
//****************************************************
void glut3DSetup() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClearDepth(1.0f);
    
    if(light) {
        glEnable(GL_LIGHTING);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Perspective Camera
    gluPerspective(FOV_Y, aspectRatio, Z_NEAR, Z_FAR);
    glMatrixMode(GL_MODELVIEW);
    
}

//****************************************************
// GLUT 2D Setup
//     - Sets all the openGL/GLUT Modes and views for
//      Rendering 2D Objects
//      - Uses an Orthographic Camera
//****************************************************
void glut2DSetup() {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
=======

//****************************************************
// GLUT 3D Setup
//     - Sets all the openGL/GLUT Modes and views for
//      Rendering 3D Objects
//****************************************************
void glut3DSetup() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClearDepth(1.0f);
    
    if(light) {
        glEnable(GL_LIGHTING);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Perspective Camera
    gluPerspective(FOV_Y, aspectRatio, Z_NEAR, Z_FAR);
    glMatrixMode(GL_MODELVIEW);
    
}

//****************************************************
// GLUT 2D Setup
//     - Sets all the openGL/GLUT Modes and views for
//      Rendering 2D Objects
//      - Uses an Orthographic Camera
//****************************************************
void glut2DSetup() {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
>>>>>>> Collisions updated
    // Orthographic Camera
    glOrtho(0.0, viewport.w, viewport.h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 

}

//****************************************************
// MyReshape 
//     - reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
    
    if(debugFunc) {
        std::cout << "myReshape Called" << std::endl;
    }

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


//****************************************************
// Print HUD
//      - Prints the Heads Up Display:
//      Variables To be Printed:
//          - Timestep
//****************************************************
void printHUD(float x, float y, float r, float g, float b, const char* text) {
    glColor3f(r,g,b);
    glRasterPos2f(x,y);
    while(*text) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *text);
        text++;
    }

}

//****************************************************
// DrawTestLine - Draws a line for each axis, (x,y,z)
//      - Red for X-axis
//      - Green for Y-axis
//      - Blue for Z-axis
//****************************************************
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
<<<<<<< HEAD

//****************************************************
// TODO: Render Stretch Springs
//****************************************************

//****************************************************
=======

//****************************************************
// TODO: Render Stretch Springs
//****************************************************

//****************************************************
>>>>>>> Collisions updated
// TODO: Render Shear Springs
//****************************************************

//****************************************************
// TODO: Render Bend Springs
//****************************************************

//****************************************************
// MyDisplay 
//     - reshape viewport if the window is resized
//****************************************************
void myDisplay() { 
    if(debugFunc) {
        std::cout << "myDisplay Called" << std::endl;
    }

    glut3DSetup();
    

    // Zeroe's Out 
    glLoadIdentity();

    // Set Camera
    glTranslatef(xTranslate, yTranslate, zTranslate);
   
    glRotatef(theta, 1.0f, 0.0f, 0.0f);
    glRotatef(phi, 0.0f, 1.0f, 0.0f);

    // Iterate through each vertex in the cloth;
   
    //drawTestLine();
   
    // Renders 3D Objects 
    renderCloth(); 
<<<<<<< HEAD
=======

    glPushMatrix();
    glTranslatef(spherePos.x,spherePos.y,spherePos.z);
    glColor3f(0.1f,0.3f,0.1f);
    glutSolidSphere(sphereRadius-0.1,50,50);

    // Clear the transforms and rotations applied earlier
    glPopMatrix();
    
>>>>>>> Collisions updated
    
   /* glut2DSetup();

    // Renders 2D Objects
    const char* output = "Theta = ";
    printHUD(-2, -2, 1.0f, 1.0f, 1.0f, output);

    glDepthMask(GL_TRUE);
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
// Step Frame - steps through one Frame
//          - Performs numTimeSteps Calculations
//          - Graph it
//****************************************************
void stepFrame() {
    for(int i = 0; i < numTimeSteps; i++) {
        
        if(gravity) {
            cloth->addExternalForce(gravityForce);
        }

<<<<<<< HEAD
        cloth->update(STEP);
=======
        cloth->update(STEP,spherePos,sphereRadius);
>>>>>>> Collisions updated

        oldTime += STEP;

    }
}


//****************************************************
// On keyPress:
//   if key:
//      'r':  Toggle Run Simulation / Pause Simulation 
//      'g':  Toggle Gravity
//      TODO:' ':  Toggle Wind
//      't':  Step through by timestep
//      's':  Toggle Flat & Smooth Shading
//      'w':  Toggle Filled & Wire Framing
//      '+':  Zoom In
//      '-':  Zoom out
//      ' ':  Close the window
//****************************************************
void keyPress(unsigned char key, int x, int y) {
    
    switch(key) {
        case 'r':
            running = !running;
            // TODO: Pause Time that is being kept track of
            // 
            
            break;
        case 'g':
            gravity = !gravity;
            std::cout << "Gravity is now ";
            if(gravity) {
                std::cout << "ON" << std::endl;
            } else {
                std::cout << "OFF" << std::endl;
            }
            // Redo Forces?
            break;
        case 't':
            if(!running) {
                stepFrame();
            }
            break;
        case 'l':
            if(light) {
                // glEnable(GL_LIGHTING);
            } else {
                // glDisable(GL_LIGHTING);
            }
            break;
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
    bool c1, c2, c3, c4;
    bool euler;

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
    
    cloth = new Cloth(width, height, corners[0], corners[1], corners[2], corners[3], euler);
    cloth->setFixedCorners(c1, c2, c3, c4);

}

int main(int argc, char *argv[]) {
   
    // Initialize GLUT
    glutInit(&argc, argv);
    
    // Process Inputs

    if(argc > 3) {
        std::cerr << "Too many arguments included" << std::endl;
        std::exit(1);
    } else {
        if(argc >= 2) {
            inputFile = argv[1];
            
            if(argc == 3) {
                if(string(argv[2])== "-v") {
                    euler = false;
                } else {
                    std::cerr << "Incorrect Flag Parameter" << std::endl;
                    std::exit(1);
                }
            } else {
                euler = true;
            }


        } else {
            euler = true;
            inputFile = "test/cloth.test";
        }
    }

    loadCloth(inputFile);

    initScene();

    // GLUT Loop    
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(keyPress);
    glutSpecialFunc(arrowKeyPress);
    glutMainLoop();

    return 0;
}



