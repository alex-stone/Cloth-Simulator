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


// OpenGL World To Camera - Transformation Variables:
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
float framesPerSecond = 30.0f;
float frameDuration; // in Milliseconds
float currentFPS = 0.0f;
int frameNum = 0;

// If true, use constant STEP value. If false use constant FPS value
bool constantStep;  

// FPS Calculation Variables
int oldFrameNum = 0;
float lastFPStime = 0.0f;

int numTimeSteps = 30;
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


//****************************************************
// Light & Material Property Functions
//****************************************************

//****************************************************
// Light Reposition
//      - Sets the position, and is called after
//          translations, so that it doesn't move 
//          w.r.t. World Coordinates
//****************************************************
void lightReposition() {
    GLfloat light_position0[] = { -2.0f, 2.0f, 1.0f, 1.0f };
    GLfloat light_position1[] = { 2.0f, 5.0f, -3.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
}

//****************************************************
// Light Setup:
//      - Sets the Light Ambient, Diffuse, Specular
//****************************************************
void lightSetup() {
    // Light Specific Values
//    GLfloat light_ambient[]=  { 0.4f, 0.6f, 0.8f, 1.0f};
//    GLfloat light_diffuse[]=  { 0.7f, 0.6f, 0.5f, 1.0f};
    
    GLfloat light_specular[] = { 0.3, 0.3, 0.3, 1.0f};
    GLfloat light_ambient[]=  { 0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat light_diffuse[]=  { 1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);        
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);     
    glLightfv(GL_LIGHT0, GL_SPECULAR,light_specular);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);        
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);     
    glLightfv(GL_LIGHT1, GL_SPECULAR,light_specular);
   
    lightReposition();

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
}

//****************************************************
// Material Setup:
//      - Sets the Material Properties
//      - Uses GL_COLOR_MATERIAL, allowing for each
//          Object drawn to set its own Ambient & 
//          Diffuse values using glColor
//****************************************************
void materialSetup() {
//    GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0};
//    GLfloat mat_shininess[] = { 60.0 };

    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = { 20.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    glEnable(GL_COLOR_MATERIAL);                // Enables Material Properties to be shown
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

}

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
  
    // Enables OpenGL to use Z-Buffer & Depth Testing
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_NORMALIZE);
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Provides Nicer Perspective Calculations   
    
    // Initializes Light & Material Property Values
    lightSetup();
    materialSetup();
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Initializes Wireframe to be ON 
    glShadeModel(GL_SMOOTH);                    // Initializes Smooth Shading

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);    // Allows for 2-Sided Lighting Calculations

    // Initialize Camera Properties:
    theta = 0.0f;
    phi = 0.0f;
    xTranslate = 0.0f;
    yTranslate = 0.0f;
    zTranslate = -5.0f;

    // Initialize Drawing Variables:
    wire = false;
    smooth = true;
    light = true;
    running = false;
    
    // Initialize Animation Variables:
    frameDuration = 1000.0f / framesPerSecond;  
    constantStep = false;

    // Initialize External Force Variables
    gravity = true;
    wind = false;
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
    glLoadIdentity(); 
}

//****************************************************
// GLUT 2D Setup
//     - Sets all the openGL/GLUT Modes and views for
//      Rendering 2D Objects
//      - Uses an Orthographic Camera
//****************************************************
void glut2DSetup() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, viewport.w, viewport.h, 0, -1, 1);
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
// Print Text:
//      - Prints Text @ (x, y) w/ color (r,g,b)
//****************************************************
void printText(float x, float y, float r, float g, float b, std::string text) {
  
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

    // Print Frame Number
    std::stringstream frameStream;
    frameStream << "Frame #: " << frameNum;
    std::string frameOut = frameStream.str();
    
    printText(5, 3*LINE_SIZE, r, g, b, frameOut);
    
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


    // Print Current Frames Per Second:
    std::stringstream fpsStream;
    fpsStream << "FPS: " << currentFPS;
    std::string fpsOut = fpsStream.str();

    printText(5, 6*LINE_SIZE, r, g, b, fpsOut);
    
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
//      - Defines Vertices CounterClockwise (CCW)
//      - Each Loop Draws a Row of the Cloth
//      Ex: 
//          1 * * * 3 * * * 5 * * * 7
//          *     * *     * *     * *
//          *   *   *   *   *   *   *
//          * *     * *     * *     *
//          2 * * * 4 * * * 6 * * * 8
//
//****************************************************
void renderCloth() {
    cloth->updateNormals();
 
    for(int h = 0; h < cloth->getHeight()-1; h++) {
        glBegin(GL_TRIANGLE_STRIP);    

        for(int w = 0; w < cloth->getWidth(); w++) {

            //glColor4f(0.0f,0.4f,0.6f,0.4f);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);


            Vertex* temp = cloth->getVertex(w, h);
            glNormal3f(temp->getNorm().x, temp->getNorm().y, temp->getNorm().z);
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);

            temp = cloth->getVertex(w, h+1);
            glNormal3f(temp->getNorm().x, temp->getNorm().y, temp->getNorm().z);
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);

        }
        glEnd();
    }
}

//****************************************************
// Update Collisions:
//      - Iterates through each Shape and tests the
//      Cloth for collisions
//****************************************************
void updateCollisions() {
    for(int i = 0; i < shapes.size(); i++) {
        cloth->updateCollision(shapes[i]);
    }
}

//****************************************************
// Pre Update Calculation:
//      - Performs all the updates that occur before
//        doing the time sensitive cloth->update
//****************************************************
void preUpdateCalculation() {

    // Note: Needs to be done before wind
    cloth->updateNormals();

    if(gravity) {
        cloth->addExternalForce(gravityForce);
    }

    if(wind) {
        cloth->addExtForce(extForce);
    }

    updateCollisions();
}


//****************************************************
// Process Calculation
//      - Performs a single Cloth Update, for the time
//          period (lastUpdateTime - currentTime) 
//****************************************************
float processCalculation(float lastUpdateTime) {
    
    preUpdateCalculation();

    float currentTime = glutGet(GLUT_ELAPSED_TIME);

    float timeChange = (currentTime - lastUpdateTime + 0.0f)/1000.0f;

    cloth->update(timeChange);

    return currentTime;

}

//****************************************************
// Process Frame:
//      - Handles a single drawn Frame, performing
//          cloth updates, until the frameDuration is
//          met.
//      - Setting lastUpdateTime to the returned value
//          ensures that the change in time passed to
//          cloth->update is the time difference from
//          the start of one update to the start of
//          the next.
//****************************************************
void processFrame() {
    float frameStartTime = glutGet(GLUT_ELAPSED_TIME);
    float lastUpdateTime = glutGet(GLUT_ELAPSED_TIME);

    while((lastUpdateTime - frameStartTime) < frameDuration) {

        lastUpdateTime = processCalculation(lastUpdateTime);

        // Set lastTimeUpdated to be right after we updated the Cloth
        //lastUpdateTime = glutGet(GLUT_ELAPSED_TIME);
    }

    frameNum++;

    // Display the Frame
    glutPostRedisplay();
}

//****************************************************
// Step Frame - steps through one Frame
//          - Performs numTimeSteps Calculations
//          - Graph it
//****************************************************
void stepFrame() {
    for(int i = 0; i < numTimeSteps; i++) {
        
        preUpdateCalculation();

        cloth->update(STEP);

    }

    // The Actual Printed Frame
    frameNum++;
}

//****************************************************
// Calc FPS:
//      - Called every time a frame is drawn
//      - After 1 seconds will calculate the number of
//          frames drawn in that time
//      - Updates the currentFPS variables which is
//          then added to the HUD
//****************************************************
void calcFPS () {

    float currentFPStime = glutGet(GLUT_ELAPSED_TIME);
    int timeChange = currentFPStime - lastFPStime;

    // If a second has elapsed
    if(timeChange > 1000) {
        currentFPS = (frameNum - oldFrameNum)/ (timeChange / 1000.0f);

        lastFPStime = currentFPStime;

        oldFrameNum = frameNum;
    }
}


//****************************************************
// MyDisplay 
//     - reshape viewport if the window is resized
//****************************************************
void myDisplay() { 
    if(debugFunc) {
        std::cout << "myDisplay Called" << std::endl;
    }

    // Sets OpenGL Variables to Render 3D
    glut3DSetup();
    
    //gluLookAt(theta,phi,0,cameraDirection[0],cameraDirection[1],cameraDirection[2],0,1,0);
    // Zeroe's Out 
    glLoadIdentity();

    // Transformation from World to Camera Coordinates
    glTranslatef(xTranslate, yTranslate, zTranslate); 
    glRotatef(phi, 1.0f, 0.0f, 0.0f);
    glRotatef(theta, 0.0f, 1.0f, 0.0f);

    // Set Light & Material Values
    lightReposition();
   
    // Renders 3D Objects 
    renderCloth(); 

    // Draw Shapes:
    for(int i = 0; i < numShapes; i++) {
        glCallList(shapeDrawLists[i]);
    }
    
    // Sets OpenGL Variables to Render 2D:
    glut2DSetup();
    printHUD();


    glFlush();
    glutSwapBuffers();
        
    if(running) {

        if(constantStep) {
            stepFrame();
            calcFPS();
            glutPostRedisplay();
        } else {
            processFrame();
            calcFPS();
        }
    }
}

//****************************************************
// Draw Shape
//      - Returns the draw list for a given shape
//      - Can handle different shapes: Spheres, Planes
//****************************************************
GLuint drawShape(Shape* s) {
  
    GLuint shapeList = glGenLists(1);
    glNewList(shapeList, GL_COMPILE);

    if(s->getType() == "SPHERE") {
        glm::vec3 center = s->getCenter();

        glPushMatrix();
        glTranslatef(center.x, center.y, center.z);
        glColor3f(0.3f, 0.3f, 0.6f);
        
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
// Process Inputs
//      - Processes Command Line Arguments and sets
//        Variables and Calls Functions appropriately
//      - Follows Format:
//      ./Scene *cloth-test* *shape-test* *
//****************************************************
void processInputs(int argc, char *argv[]) {
  
    // Help Flag
    if(argc == 2 && string(argv[1]) == "-help" || string(argv[1]) == "--help") {
        std::cout << std::endl;
        std::cout << "USAGE: ./Scene <cloth_file> <shape_file> [OPTIONAL]" << std::endl;
        std::cout << "OPTIONAL: '-v' = Verlet Integration" << std::endl;
        std::cout << "          blank = Euler Integration" << std::endl;
        std::cout << std::endl;
        std::exit(1);
    }    

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
            // Reset's the Cloth to initial position
            cloth = NULL;
            loadCloth(inputFile);
            frameNum = 0;

            break;
        case 'r':
            running = !running;

            break;
        case 'c':
            constantStep = !constantStep;
            
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
    
    // Process Inputs & sets variables: inputFule, shapeFile, euler
    processInputs(argc, argv);

    // Loads Cloth & Shapes Info
    loadCloth(inputFile);
    loadShapes(shapeFile);

    // Initializes Window & OpenGL Settings
    initScene();

    std::cout << glutGet(GLUT_ELAPSED_TIME) << std::endl;

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



