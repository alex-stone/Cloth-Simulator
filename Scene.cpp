#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>

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

#include "glm/glm.hpp"
#include "Cloth.h"
#include "Vertex.h"
#include "Shape.h"
#include "Sphere.h"
#include "Plane.h"

#define PI 3.14159265

using namespace std;

class Viewport {
  public:
    int w, h; // width and height
};

//****************************************************
// Global Variables 
//****************************************************
Viewport                viewport;
Cloth*                  cloth;
const char*             inputFile;
const char*             shapeFile;
std::vector<Shape*>     shapes;
GLuint*                 shapeDrawLists;
int                     numShapes;

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

    GLfloat LightPosition[]=    { -2.0f, 2.0f,0, 0.0f };
    GLfloat LightPosition1[]=    { 2.0f, -2.0f,0, 0.0f };

// OpenGL Transformation Variables:
GLfloat theta;
GLfloat phi;
GLfloat xTranslate;
GLfloat yTranslate;
GLfloat zTranslate;

GLfloat cameraDirection[] = {xTranslate,yTranslate,zTranslate};
// OpenGL Transformation Constants;
const GLfloat TRANSLATE_INC = 0.05f;
const GLfloat Z_TRANSLATE_INC = 0.2f;
const GLfloat ROTATE_INC = 3.0f;

// Animation Variables:
float timestep = 0;
float oldTime = 0;


int numTimeSteps = 30;//30;
const float STEP = 0.008f;

// Position Update Method Variables: Command Lines
bool euler;

// Forces:
bool gravity;
glm::vec3 gravityForce(0.0f, -1.0f, 0.0f);

//TODO: WIND INFO
bool wind;
glm::vec3 extForce(0.0f, 0.0f, -1.0f);
//glm::vec3 extForce(1.0f, -0.7f, 1.0f);

// Debug Variables:
bool debugFunc = false;

// HUD Variables;
const int LINE_SIZE = 15;


// Want a key to step through the Animation


//****************************************************
// Glut Functions
//****************************************************

void initScene() {

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

    // Initialize Viewport Size
    viewport.w = 800;
    viewport.h = 800;

    // The Size and Position of the Window
    glutInitWindowSize(viewport.w, viewport.h);
    glutInitWindowPosition(0,0);
    glutCreateWindow("CS184 - Final Project");
  
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_NORMALIZE);


    // Initialize Camera Properties:
    theta = 0.0f;
    phi = 0.0f;
    xTranslate = 0.0f;
    yTranslate = 0.0f;
    zTranslate = -10.0f;

    // Initialize Drawing Variables:
    wire = false;
    smooth = true;
    light = true;
    running = false;
    
    gravity = true;
    wind = false;

    // Set up Lights:
    //glEnable(GL_CULL_FACE);
    
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
   
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {40.0 };
    GLfloat LightAmbient[]=  { 0.4f, 0.6f, 0.8f, 0.4f};
    GLfloat LightDiffuse[]=  { 0.7f, 0.6f, 0.5f, 0.5f};
    GLfloat light_specular[] = { 0.4, 0.3, 0.8, 0.6};
    
     glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);        // Setup The Ambient Light
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);     // Setup The Diffuse Light
    glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);
    glLightfv(GL_LIGHT0, GL_SPECULAR,light_specular);
     glLightfv(GL_LIGHT1, GL_POSITION,LightPosition1);
    //glEnable(GL_LIGHT1);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
      
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
   

   
    glEnable(GL_COLOR_MATERIAL);

    // Initializes Wireframe to be OFF
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_SMOOTH);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    
  
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);

    glDepthFunc(GL_LEQUAL);
    

}

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
    } else {
        glDisable(GL_LIGHTING);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Perspective Camera
    gluPerspective(FOV_Y, aspectRatio, Z_NEAR, Z_FAR);
    glMatrixMode(GL_MODELVIEW);
     //gluLookAt(cameraDirection[0],cameraDirection[1],cameraDirection[2],theta,phi,0,0,1,0);

    glLoadIdentity(); 
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
    
    // Orthographic Camera
    glOrtho(0.0, viewport.w, viewport.h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
//       gluLookAt(cameraDirection[0],cameraDirection[1],cameraDirection[2],theta,phi,0,0,1,0);

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
// Print Text:
//      - Prints Text @ (x, y) w/ color (r,g,b)
//****************************************************
void printText(float x, float y, float r, float g, float b, std::string text) {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glColor3f(r,g,b);
    glRasterPos2f(x,y);

    for(int i = 0; i < text.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
    }

}

//****************************************************
// Print HUD
//      - Prints the Heads Up Display:
//      Variables To be Printed:
//          - Timestep
//****************************************************
void printHUD() {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;

    // Print Theta:
    int thetaAngle = theta;
    thetaAngle = thetaAngle%360;
    std::stringstream thetaStream;
    thetaStream << "Theta: " << thetaAngle;
    std::string thetaOut = thetaStream.str();
    
    printText(5, LINE_SIZE, r, g, b, thetaOut); 

    // Print Phi 
    int phiAngle = phi;
    phiAngle = phiAngle%360;
    std::stringstream phiStream;
    phiStream << "Phi: " << phiAngle;
    std::string phiOut = phiStream.str();
    
    printText(5, 2*LINE_SIZE, r, g, b, phiOut);

    // Print Frames Per Second

    // Print Time:
    int t = timestep;
    std::stringstream timeStream;
    timeStream << "Time Step: " << t;
    std::string timeOut = timeStream.str();
    
    printText(5, 3*LINE_SIZE, r, g, b, timeOut);
    
    // Print Gravity:
    std::string gravBool;
    if(gravity) {
        gravBool = "ON";
    } else {
        gravBool = "OFF";
    }

    std::stringstream gravStream;
    gravStream << "Gravity: " << gravBool;
    std::string gravOut = gravStream.str(); 

    printText(5, 4*LINE_SIZE, r, g, b, gravOut);

    // Print Wind:
    std::string windBool;
    if(wind) {
        windBool = "ON";
    } else {
        windBool = "OFF";
    }

    std::stringstream windStream;
    windStream << "Wind: " << windBool;
    std::string windOut = windStream.str();

    printText(5, 5*LINE_SIZE, r, g, b, windOut);
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
    cloth->updateNormals();
    /*glEnable(GL_LIGHTING);
    GLfloat Specular[] = {0.0f,0.2f,0.8f};
    GLfloat Ambient[]= { 0.0f, 0.2f, 0.7f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ambient);    //set material
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Specular);
   
    */
 
    for(int h = 0; h < cloth->getHeight()-1; h++) {
        glBegin(GL_TRIANGLE_STRIP);    
        for(int w = 0; w < cloth->getWidth(); w++) {

            //if((w+h)%2 ==0 ){
          
                glColor4f(0.0f,0.4f,0.6f,0.4f);
           /*}
           else{
            glColor3f(0.0f,0.0f,0.0f);
           }*/
           
           

            Vertex* temp = cloth->getVertex(w, h);
            glNormal3f(temp->getNorm().x, temp->getNorm().y, temp->getNorm().z);
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);

            temp = cloth->getVertex(w, h+1);
            glNormal3f(temp->getNorm().x, temp->getNorm().y, temp->getNorm().z);
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);

        }

        glEnd();

    }
    //glDisable(GL_LIGHTING);
}


void updateCollisions() {
    for(int i = 0; i < shapes.size(); i++) {
        cloth->updateCollision(shapes[i]);
    }
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

        if(wind) {
            cloth->addExtForce(extForce);
        }

        cloth->updateNormals();
        updateCollisions();
        cloth->update(STEP);
        //updateCollisions();

        oldTime += STEP;
    }
    //cloth->updateNormals();
    timestep++;

}

//****************************************************
// MyDisplay 
//     - reshape viewport if the window is resized
//****************************************************
void myDisplay() { 
    if(debugFunc) {
        std::cout << "myDisplay Called" << std::endl;
    }

    glut3DSetup();
    
    //gluLookAt(cameraDirection[0],cameraDirection[1],cameraDirection[2],theta,phi,0,0,1,0);
    // Zeroe's Out 
    glLoadIdentity();

    // Set Camera
    glTranslatef(xTranslate, yTranslate, zTranslate);

  
   
    glRotatef(phi, 1.0f, 0.0f, 0.0f);
    glRotatef(theta, 0.0f, 1.0f, 0.0f);

    // Iterate through each vertex in the cloth;
   
    //drawTestLine();
   
    // Renders 3D Objects 
    renderCloth(); 

    // Draw Shapes:
    for(int i = 0; i < numShapes; i++) {
        glCallList(shapeDrawLists[i]);
    }
    
    //glut2DSetup();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, viewport.w, viewport.h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    //gluLookAt(cameraDirection[0],cameraDirection[1],cameraDirection[2],theta,phi,0,0,1,0);
    glLoadIdentity();
        
    printHUD();


    if(running) {
        stepFrame();
    }


    // Renders 2D Objects
    //const char* output = "Theta = ";
    //printHUD(5,5 , 1.0f, 1.0f, 1.0f, output);
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
    
    if(running) {
        glutPostRedisplay();
    }
}



void runLoop() {
    while(running) {
        stepFrame();
        myDisplay();

        
    }

}

GLuint drawShape(Shape* s) {
  
    GLuint shapeList = glGenLists(1);
    glNewList(shapeList, GL_COMPILE);

    
    if(s->getType() == "SPHERE") {
        glm::vec3 center = s->getCenter();

        glPushMatrix();
        glTranslatef(center.x, center.y, center.z);

        glColor3f(0.1f, 0.3f, 0.1f);
        
        glutSolidSphere(s->getRadius()-0.1 ,50 ,50 );
        glPopMatrix();
    } 

    if(s->getType() == "PLANE") {

        glm::vec3 norm = s->getNormal();
        glm::vec3 point = s->getUL();


        glPushMatrix();

        glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f);
        glNormal3f(norm.x, norm.y, norm.z);

        glVertex3f(point.x, point.y, point.z );
        
        point = s->getUR();
        glVertex3f(point.x, point.y, point.z );

        point = s->getLR();
        glVertex3f(point.x, point.y, point.z );
        
        point = s->getLL();
        glVertex3f(point.x, point.y, point.z );

        glEnd();
        glPopMatrix();
  
    }


    glEndList();

    return shapeList;
}


//****************************************************
// Make Draw Lists Function 
//      - Creates the draw list for all the shapes
//****************************************************
void makeDrawLists() {
    for(int i = 0; i < numShapes; i++) {
        shapeDrawLists[i] = drawShape(shapes[i]);
    }
}

//****************************************************
// LoadShapes Function
//      - Creates the draw list for all the shapes
//****************************************************
void loadShapes(const char* shapeInput) {
    std::ifstream inpfile(shapeInput, ifstream::in);

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
        case 'y':
            // Reset:
            cloth = NULL;
            loadCloth(inputFile);
            oldTime = 0;
            timestep = 0;

            break;
        case 'r':
            running = !running;

            
            // TODO: Pause Time that is being kept track of
            // 
            
            break;
        case 's':
            smooth = !smooth;
            if(smooth) {
                glShadeModel(GL_SMOOTH);
            } else {
                glShadeModel(GL_FLAT);
            }
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
        case 'f':
            wind = !wind;
            break;
        case 't':
            if(!running) {
                stepFrame();
            }
            break;
        case 'l':
            light = !light;
            if(light) {
                 glEnable(GL_LIGHTING);
            } else {
                 glDisable(GL_LIGHTING);
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
                theta -= ROTATE_INC;
            }
            break;
        case 101:   // Up Arrow
            if(shift) {
                yTranslate += TRANSLATE_INC;
            } else {
                phi -= ROTATE_INC;
            }
            break;
        case 102:   // Right Arrow
            if(shift) {
                xTranslate += TRANSLATE_INC;
            } else {
                theta += ROTATE_INC;
            }
            break;
        case 103:   // Down Arrow
            if(shift) {
                yTranslate -= TRANSLATE_INC;
            } else {
                phi += ROTATE_INC;
            }
            break;
    }

    myDisplay();
}


int main(int argc, char *argv[]) {
   
    // Initialize GLUT
    glutInit(&argc, argv);
    
    // Process Inputs

    if(argc > 4) {
        std::cerr << "Too many arguments included" << std::endl;
        std::exit(1);
    } else {
        if(argc >= 3) {
            inputFile = argv[1];
            shapeFile = argv[2];

            if(argc == 4) {
                if(string(argv[3])== "-v") {
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
            shapeFile = "shapes/4spheres.test";
        }
    }

    // Loads Cloth & Shapes Info
    loadCloth(inputFile);
    loadShapes(shapeFile);

    initScene();

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
}



