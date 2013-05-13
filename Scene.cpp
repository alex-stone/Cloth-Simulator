#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include "FreeImage.h"

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
bool wire;          // Draw Springs vs. not springs
bool smooth;
bool running;       // Is simulation running in real-time or paused for step through
bool light;

// Output Photo Flag
bool saveImage;

// Options Menu Drawing Variables
bool showOptions;
bool reopenOptions;     // Indicates to reopen Options menu if resized to width > 700.

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
int cameraNum = 0;

float calcsPerFrame = 0.0f;
int numCalculations = 0;

// Drawing Cloth Structure Variables:
bool spherePoints = true;


// If true, use constant STEP value. If false use constant FPS value
bool constantStep;  

// FPS Calculation Variables
int oldFrameNum = 0;
float lastFPStime = 0.0f;

int numTimeSteps = 15;
const float STEP = 0.005f;

const float STEP_INC = 0.001f;
float timestep = 0.005f;

// Position Update Method Variables: Command Lines
bool euler;

// Forces:
bool gravity;
glm::vec3 gravityAccel(0.0f, -9.81f, 0.0f);// -9.81f, 0.0f);

//TODO: WIND INFO
bool wind;
glm::vec3 windForce(0.0f, 0.0f, -1.0f);

float windScale = 1.0f;
float windINC = 0.4f;

// Debug Variables:
bool debugFunc = false;
bool debugStats = true;

// HUD Variables;
const int LINE_SIZE = 15;
const int LARGE_LINE_SIZE = 20;

// Texture Variables:
std::string currentTexture="";
GLuint texture0;
GLuint texture1;
GLuint texture2;

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

    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = { 20.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    glEnable(GL_COLOR_MATERIAL);                // Enables Material Properties to be shown
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

}

//****************************************************
// Texture Setup Functions:
//      - Sets the settings, to correctly show the 
//          textures mapped onto the cloth.
//****************************************************

void textureSetup() {

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

//****************************************************
// Load Texture:
//      - Loads a single texture from a file
//****************************************************
void loadTexture(const char* filename) {
    int width, height;
    unsigned char* data;
    FILE* file;

    width = 256;
    height = 256;

    data = (unsigned char*) malloc(width*height*4);

    file = fopen(filename, "rb");

    if( file == NULL) {
        return;
    }

    fread(data, width * height * 4, 1, file);
    fclose(file);

    textureSetup();

    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data);
}

//****************************************************
// Initialize Textures:
//      - Generates 'names' for each texture, and then
//          binds the textures so they can be loaded.
//****************************************************
void initTextures() {
    glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    loadTexture("textures/texture0.raw");

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    loadTexture("textures/texture1.raw");

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    loadTexture("textures/texture2.raw");

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
    initTextures();

    // Initialize which Texture to Use
    glBindTexture(GL_TEXTURE_2D, texture0);
    currentTexture = "CHECKERBOARD";
    
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
    showOptions = false;
    
    // Initialize Animation Variables:
    frameDuration = 1000.0f / framesPerSecond;  
    constantStep = false;

    // Initialize External Force Variables
    gravity = true;
    wind = false;

    saveImage = false;
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

    /*
    if(wire) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }*/

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
    glDisable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

    // If the window was resized from a smaller size and Options were open, then reopen.
    if(w > 700 && reopenOptions && !saveImage) {

        showOptions = true;
        reopenOptions = false;
    }

    // If Options were open, and window resized to a window smaller than 700, set it to reopen
    if(w < 700 && showOptions) {
        showOptions = false;
        reopenOptions = true;
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
void printText(float x, float y, float r, float g, float b, std::string text, void *font) {
  
    glColor3f(r,g,b);
    glRasterPos2f(x,y);

    for(int i = 0; i < text.length(); i++) {
        glutBitmapCharacter(font, text[i]);
    }

}

//****************************************************
// Print HUD
//      - Prints the Heads Up Display:
//      - Variables: Angles, FrameNum, Gravity?, wind?
//          windForce, FPS
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
    
    printText(5, LINE_SIZE, r, g, b, thetaOut, GLUT_BITMAP_HELVETICA_12); 

    // Print Phi 
    int phiAngle = phi;
    phiAngle = phiAngle%360;
    std::stringstream phiStream;
    phiStream << "Phi: " << phiAngle;
    std::string phiOut = phiStream.str();
    
    printText(5, 2*LINE_SIZE, r, g, b, phiOut, GLUT_BITMAP_HELVETICA_12);

    // Print Frame Number
    std::stringstream frameStream;
    frameStream << "Frame #: " << frameNum;
    std::string frameOut = frameStream.str();
    
    printText(5, 3*LINE_SIZE, r, g, b, frameOut, GLUT_BITMAP_HELVETICA_12);
    
    // Print Gravity:
    std::string gravOut;
    if(gravity) {
        gravOut = "Gravity: ON";
    } else {
        gravOut = "Gravity: OFF";
    }

    printText(5, 4*LINE_SIZE, r, g, b, gravOut, GLUT_BITMAP_HELVETICA_12);

    // Print Wind:
    std::string windOut;
    if(wind) {
        windOut = "Wind: ON";
    } else {
        windOut = "Wind: OFF";
    }

    printText(5, 5*LINE_SIZE, r, g, b, windOut, GLUT_BITMAP_HELVETICA_12);

    std::stringstream windForceStream;

    // Print Wind Force
    windForceStream << "Wind Force: " << glm::length(windForce);
    std::string windForceOut = windForceStream.str();

    printText(5, 6*LINE_SIZE, r, g, b, windForceOut, GLUT_BITMAP_HELVETICA_12);

    // Print Current Frames Per Second:
    std::stringstream fpsStream;
    fpsStream << "FPS: " << currentFPS;
    std::string fpsOut = fpsStream.str();

    printText(5, 7*LINE_SIZE, r, g, b, fpsOut, GLUT_BITMAP_HELVETICA_12);

    // Print Calculations Per Frame:
    std::stringstream calcStream;
    calcStream << "Calcs Per Frame: " << calcsPerFrame;
    std::string calcOut = calcStream.str();

    printText(5, 8*LINE_SIZE, r, g, b, calcOut, GLUT_BITMAP_HELVETICA_12);

    
}


//****************************************************
// Print Options Helpers:
//      - Each one Prints a Column

//****************************************************
void displayPerformance(int leftBound, int upBound, glm::vec3 color) {

    //Print Performance Header
    printText(leftBound+5, upBound + LARGE_LINE_SIZE, color.x, color.y, color.z, "INTEGRATION: ", GLUT_BITMAP_HELVETICA_18);

    // Print Integration Type:
    std::string intType;
    if(euler) {
        intType = "Method: Euler Integration";
    } else {
        intType = "Method: Verlet Integration";
    }

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + LINE_SIZE, color.x, color.y, color.z, intType, GLUT_BITMAP_HELVETICA_12);

    // Toggle Constant FPS v.s. Constant Timestep (Variable v.s. Fixed Timestep)
    std::string constantOut;

    if(constantStep) {
        constantOut = "Toggle Timestep (C): Fixed";
    } else {
        constantOut = "Toggle Timestep (C): Variable";
    }

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 2*LINE_SIZE, color.x, color.y, color.z, constantOut, GLUT_BITMAP_HELVETICA_12);

    // Print Timestep
    std::stringstream stepStream;
    stepStream << "Timestep: " << timestep << "s";
    std::string stepOut = stepStream.str();

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 3*LINE_SIZE, color.x, color.y, color.z, stepOut, GLUT_BITMAP_HELVETICA_12);
 
    // Print how to Increase Timestep
    std::string incStep = "Increase Timestep: (Y)";
    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 4*LINE_SIZE, color.x, color.y, color.z, incStep, GLUT_BITMAP_HELVETICA_12);
 
    // Print How to decrease Timestep
    std::string decStep = "Decrease Timestep: (U)";
    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 5*LINE_SIZE, color.x, color.y, color.z, decStep, GLUT_BITMAP_HELVETICA_12);
 
}

void displayForce(int leftBound, int upBound, glm::vec3 color) {
    
    // Print Force Header:
    printText(leftBound+5, upBound + LARGE_LINE_SIZE, color.x, color.y, color.z, "FORCES: ", GLUT_BITMAP_HELVETICA_18);

    // Print Gravity Info:
    std::string gravBool;
    if(gravity) {
        gravBool = "ON";
    } else {
        gravBool = "OFF";
    }

    std::string gravOut = "Toggle Gravity (G): " + gravBool;

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + LINE_SIZE, color.x, color.y, color.z, gravOut, GLUT_BITMAP_HELVETICA_12);

    // Print Wind Info:
    std::string windBool;
    if(wind) {
        windBool = "ON";
    } else {
        windBool = "OFF";
    }

    std::string windOut = "Toggle Wind (F): " + windBool;

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 2*LINE_SIZE, color.x, color.y, color.z, windOut, GLUT_BITMAP_HELVETICA_12);

    // Print Wind Direction
    std::stringstream windDirStream;
    windDirStream << "Wind Direction: " << "(" << windForce.x << ", " << windForce.y << ", " << windForce.z << ")";
    std::string windDirOut = windDirStream.str();

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 3*LINE_SIZE, color.x, color.y, color.z, windDirOut, GLUT_BITMAP_HELVETICA_12);

    // Print Wind Force Magnitude
    std::stringstream windMagStream;
    windMagStream << "Wind Magnitude: " << glm::length(windForce);
    std::string windMagOut = windMagStream.str();

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 4*LINE_SIZE, color.x, color.y, color.z, windMagOut, GLUT_BITMAP_HELVETICA_12);

    std::string windIncrease = "Increase Wind Force: (J)";
    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 5*LINE_SIZE, color.x, color.y, color.z, windIncrease, GLUT_BITMAP_HELVETICA_12);
    
    std::string windDecrease = "Decrease Wind Force: (K)";
    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 6*LINE_SIZE, color.x, color.y, color.z, windDecrease, GLUT_BITMAP_HELVETICA_12);


}

void displayShading(int leftBound, int upBound, glm::vec3 color) {
    
    // Print Shading Header:
    printText(leftBound+5, upBound + LARGE_LINE_SIZE, color.x, color.y, color.z, "SHADING: ", GLUT_BITMAP_HELVETICA_18);

    // Print Smooth Shading Info:
    std::string smoothOut;
    if(smooth) {
        smoothOut = "Smooth Shading: ON";
    } else {
        smoothOut = "Smooth Shading: OFF";
    }

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + LINE_SIZE, color.x, color.y, color.z, smoothOut, GLUT_BITMAP_HELVETICA_12);

   // Print Lighting Info:
    std::string lightOut;
    if(light) {
        lightOut = "Lighting: ON";
    } else {
        lightOut = "Lighting: OFF";
    }

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 2*LINE_SIZE, color.x, color.y, color.z, lightOut, GLUT_BITMAP_HELVETICA_12);

       // Print Smooth Shading Info:
    std::string wireOut;
    if(wire) {
        wireOut = "Spring Wireframe: ON";
    } else {
        wireOut = "Spring Wireframe: OFF";
    }

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 3*LINE_SIZE, color.x, color.y, color.z, wireOut, GLUT_BITMAP_HELVETICA_12);
    
    // Print Current texture
    std::string textureOut = "Texture: " + currentTexture;
    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 4*LINE_SIZE, color.x, color.y, color.z, textureOut, GLUT_BITMAP_HELVETICA_12);

    // Print How to Change Textures
    std::string changeTexOut = "Change Texture: 1, 2";
    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 5*LINE_SIZE, color.x, color.y, color.z, changeTexOut, GLUT_BITMAP_HELVETICA_12);


}

void displayCamera(int leftBound, int upBound, glm::vec3 color) {

    // Print Camera Header:
    printText(leftBound+5, upBound + LARGE_LINE_SIZE, color.x, color.y, color.z, "SIMULATION: ", GLUT_BITMAP_HELVETICA_18);

    // Print Running Info:
    std::string runOut;
    if(running) {
        runOut = "Toggle Running (R):  ON";
    } else {
        runOut = "Toggle Running (R):  OFF";
    }

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + LINE_SIZE, color.x, color.y, color.z, runOut, GLUT_BITMAP_HELVETICA_12);

    // Frame Number:
    std::stringstream frameStream;
    frameStream << "Frame Number: " << frameNum;
    std::string frameOut = frameStream.str();

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 2*LINE_SIZE, color.x, color.y, color.z, frameOut, GLUT_BITMAP_HELVETICA_12);


    // Print Frames Per Second
    std::stringstream fpsStream;
    fpsStream << "FPS: " << currentFPS;
    std::string fpsOut = fpsStream.str();

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 3*LINE_SIZE, color.x, color.y, color.z, fpsOut, GLUT_BITMAP_HELVETICA_12);


    // Print Calculations Per Frame:
    std::stringstream calcStream;
    calcStream << "Calcs Per Frame: " << calcsPerFrame;
    std::string calcOut = calcStream.str();

    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 4*LINE_SIZE, color.x, color.y, color.z, calcOut, GLUT_BITMAP_HELVETICA_12);

    // Print Theta:
    int thetaAngle = theta;
    thetaAngle = thetaAngle%360;
    std::stringstream thetaStream;
    thetaStream << "Theta Angle: " << thetaAngle;
    std::string thetaOut = thetaStream.str();
    
    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 5*LINE_SIZE, color.x, color.y, color.z, thetaOut, GLUT_BITMAP_HELVETICA_12);

    // Print Phi 
    int phiAngle = phi;
    phiAngle = phiAngle%360;
    std::stringstream phiStream;
    phiStream << "Phi Angle: " << phiAngle;
    std::string phiOut = phiStream.str();
    
    printText(leftBound, upBound + LARGE_LINE_SIZE + 5 + 6*LINE_SIZE, color.x, color.y, color.z, phiOut, GLUT_BITMAP_HELVETICA_12);



}

void printSideOptions() {
    glm::vec3 color (0.0f, 0.0f, 0.0f);

    int rightBound = 150;

    glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);

        glVertex2f(0,0);
        glVertex2f(0,viewport.h);
        glVertex2f(rightBound, viewport.h);
        glVertex2f(rightBound, 0);

    glEnd();

    displayPerformance(5, 5, color);
    displayForce(5, 200, color);
    displayShading(5, 500, color);
    displayCamera(5, 700, color);

}

//****************************************************
// Print Options:
//      - Prints out all of the Different Buttons
//      - And what they do
//****************************************************
void printOptions() {

    // Black Text for White Background
    glm::vec3 color(0.0f, 0.0f, 0.0f);

    int topHeight = viewport.h - 150;

    // Print Background:
    glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);

        glVertex2f(0, viewport.h-150);
        glVertex2f(0, viewport.h);
        glVertex2f(viewport.w, viewport.h);
        glVertex2f(viewport.w, viewport.h-150);
    glEnd();

    // Sets width of each Section
    int width = viewport.w/4;

    // Draw each individual Section
    displayPerformance(5, topHeight, color);
    displayCamera(width+5, topHeight, color);
    displayForce(2*width + 5, topHeight, color);
    displayShading(3*width+5, topHeight, color);
    
    // Draws Lines Around each section.
    glBegin(GL_LINES);
        glVertex2f(0, topHeight);
        glVertex2f(viewport.w, topHeight);

        glVertex2f(width, topHeight);
        glVertex2f(width, viewport.h);

        glVertex2f(2*width, topHeight);
        glVertex2f(2*width, viewport.h);

        glVertex2f(3*width, topHeight);
        glVertex2f(3*width, viewport.h);

        glVertex2f(0, topHeight+25);
        glVertex2f(viewport.w, topHeight+25);

    glEnd();

}

//****************************************************
// Print Options Header:
//      - Prints the Header for the Options Bar
//      - Always shows, even if Options Bar isn't
//          showing, so has 2 possible positions.
//****************************************************
void printOptionsHeader() {

    glm::vec3 color(0.0f, 0.0f, 0.0f);

    int bottom;
    int middle = viewport.w / 2;
    int height = 30;

    // Determine Where to start drawing
    if(showOptions) {
        bottom = viewport.h - 150;
    } else {
        bottom = viewport.h;
    }

    // Print Background
    glBegin(GL_QUADS);
        glColor3f(0.543f, 0.535f, 0.535f);

        glVertex2f(0, bottom - height);
        glVertex2f(0, bottom);
        glVertex2f(viewport.w, bottom);
        glVertex2f(viewport.w, bottom - height);
    
    glEnd();

    // Print's Title
    printText(5, bottom - 10, color.x, color.y, color.z, "TOGGLE OPTIONS: (O) ", GLUT_BITMAP_HELVETICA_18);
    
    if(saveImage) {
        printText(viewport.w-150, bottom - 10, 1.0f, 0.0f, 0.0f, "RECORDING (P)", GLUT_BITMAP_HELVETICA_18);
    } else {
        printText(viewport.w-200, bottom - 10, color.x, color.y, color.z, "NOT", GLUT_BITMAP_HELVETICA_18);
        printText(viewport.w-150, bottom - 10, color.x, color.y, color.z, "RECORDING (P)", GLUT_BITMAP_HELVETICA_18);
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
// Draw Cloth Points
//     - Draws all of the points in the cloth
//****************************************************
void drawClothPoints() {

    float radius = cloth->getPointDrawSize();

    if(spherePoints) {

         for(int h = 0; h < cloth->getHeight(); h++) {

            for(int w = 0; w < cloth->getWidth(); w++) {
                Vertex* vert = cloth->getVertex(w, h);

                glColor3f(1.0, 1.0, 1.0);

                glm::vec3 center = vert->getPos();

                glPushMatrix();
                glTranslatef(center.x, center.y, center.z);
                
                glutSolidSphere(radius, 10 , 10 );
                glPopMatrix();

            }
        }

    } else {

        glEnable(GL_POINT_SMOOTH);

        glBegin(GL_POINTS);

        glPointSize(100.0f);


        for(int h = 0; h < cloth->getHeight(); h++) {

            for(int w = 0; w < cloth->getWidth(); w++) {
                Vertex* vert = cloth->getVertex(w, h);

                glColor3f(1.0, 1.0, 1.0);

                glm::vec3 v = vert->getPos();

                glVertex3f(v.x, v.y, v.z);

            }
        }
        glEnd();

    }
}

//****************************************************
// Draw Stretch Springs
//     - Draws all of the stretch springs
//****************************************************
void drawStretchSprings() {

    vector<Spring*> temp = cloth->getStretchSprings();

    glPushMatrix();
    glBegin(GL_LINES);

    glColor3f(1.0f, 0.0f, 0.0f);


    for(int i = 0; i < temp.size(); i++) {
        glm::vec3 p1 = temp[i]->getPos1();
        glm::vec3 p2 = temp[i]->getPos2();

        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);

    }

    glEnd();
    glPopMatrix();


}

//****************************************************
// Draw Cloth Points
//     - Draws all of the shear springs in cloth
//****************************************************
void drawShearSprings() {
    vector<Spring*> temp = cloth->getShearSprings();

    glBegin(GL_LINES);

    glColor3f(0.0f, 1.0f, 0.0f);


    for(int i = 0; i < temp.size(); i++) {
        glm::vec3 p1 = temp[i]->getPos1();
        glm::vec3 p2 = temp[i]->getPos2();

        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);

    }

    glEnd();

}

//****************************************************
// Draw Cloth Points
//      - Draws all of the bend springs in the cloth
//      - Not useful visually until out of plane
//          movement
//****************************************************
void drawBendSprings() {
    vector<Spring*> temp = cloth->getBendSprings();

    glBegin(GL_LINES);

    glColor3f(0.0f, 0.0f, 1.0f);


    for(int i = 0; i < temp.size(); i++) {
        glm::vec3 p1 = temp[i]->getPos1();
        glm::vec3 p2 = temp[i]->getPos2();

        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);

    }

    glEnd();
}

//****************************************************
// Render Cloth Structure
//      - Renders each vertex of the cloth
//      - Renders The Springs based on variables:
//             - shear, stretch, bend
//****************************************************
void renderClothStructure() {

    // Draw the Bend Springs first so that the Stretch are drawn over it until the cloth goes out of plane
    drawBendSprings();

    // Draw the Stretch Springs
    drawStretchSprings();

    // Draw the Shear Springs
    drawShearSprings();

    drawClothPoints();
 
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

    if(debugFunc) {
        cout << "Render Cloth Called" << endl;
    }

    // Update Normals to be drawn
    cloth->updateNormals();

    if(debugFunc) {
        cout << "Successfully Update Normals" << endl;
    }
 
    for(int h = 0; h < cloth->getHeight()-1; h++) {
        glBegin(GL_TRIANGLE_STRIP);    

        for(int w = 0; w < cloth->getWidth(); w++) {

            //glColor4f(0.0f,0.4f,0.6f,0.4f);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

            Vertex* temp = cloth->getVertex(w, h);

            // Sets Normal Values from Vertex.
            glNormal3f(temp->getNorm().x, temp->getNorm().y, temp->getNorm().z);

            // Sets the Texture Coordinate Values to map to Current Texture
            float s = (float) w / (float) (cloth->getWidth()-1);
            float t = (float) h / (float) (cloth->getHeight()-1);
            glTexCoord2d(s,t);

            // Sets actual Vertex
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);

            // Vertex 2:
            temp = cloth->getVertex(w, h+1);

            // Vertex 2's Normal values
            glNormal3f(temp->getNorm().x, temp->getNorm().y, temp->getNorm().z);

            // Texture Coordinates, only t value changes.
            t = (float) (h+1) / (float) (cloth->getHeight()-1);

            glTexCoord2d(s,t);
            glVertex3f(temp->getPos().x, temp->getPos().y, temp->getPos().z);

        }
        glEnd();
    }

    if(debugFunc) {
        std:cout << "Render Cloth Completed" << std::endl;
    }

}


//****************************************************
// Write Image
//      - Reads in the Buffer and outputs it to an
//        image.
//****************************************************
void writeImage(int imageNum) {
    // Do Stuff

    // Note: Bitmaps have their origin (0, 0) at bottom left, and our (0,0) is top left

    std::string fileName = "test.png";

    FreeImage_Initialise();
    
    FIBITMAP* bitmap = FreeImage_Allocate(viewport.w, viewport.h, 24);
    RGBQUAD color;

    int W = viewport.w;
    int H = viewport.h;

    char *pixel_data = new char[3*W*H];

    glReadBuffer(GL_FRONT);
    glReadPixels(0, 30, W, H, GL_BGR, GL_UNSIGNED_BYTE, pixel_data);

    // Iterate through pixelBucket and do FreeImage.SetPixelColor
    // Note that FreePixel's Blue and Red are switched.
    for(int i = 0; i < viewport.w; i++) {
        for(int j = 0; j < viewport.h; j++) {

        color.rgbRed = pixel_data[i*(viewport.h*3) + j*3];
        color.rgbGreen = pixel_data[i*viewport.h*3 + j*3+1];
        color.rgbBlue = pixel_data[i*viewport.h*3 + j*3+2];

        FreeImage_SetPixelColor(bitmap, j, i, &color);
        }

    } 

    int count = cameraNum;

    std::stringstream frameStream;
    frameStream << "images/frame" << count << ".png";
    std::string frameOut = frameStream.str();

    if (FreeImage_Save(FIF_PNG, bitmap, frameOut.c_str(), 0))
    cout << "Image Successfuly Saved to file: " << frameOut << "!" << std::endl;

    FreeImage_DeInitialise();

    cameraNum++;
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
    // Clear Acceleration Here:
   // cloth->resetAccel();

    // Note: Needs to be done before wind
    //cloth->updateNormals();

    if(gravity) {
        cloth->addConstantAccel(gravityAccel);
    }

    if(wind) {
        cloth->updateNormals(); // Only update Normals if NEEDED
        cloth->addTriangleForce(windForce);
    }

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

    updateCollisions();

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

        if(glutGet(GLUT_ELAPSED_TIME) - lastUpdateTime > 1) {

            lastUpdateTime = processCalculation(lastUpdateTime);

            numCalculations++;

        }


        // Set lastTimeUpdated to be right after we updated the Cloth
        //lastUpdateTime = glutGet(GLUT_ELAPSED_TIME);

    }
    if(saveImage) {
        writeImage(frameNum);
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

        // 
        cloth->update(timestep);

        updateCollisions();

        numCalculations++;
    }

    if(saveImage) {
        writeImage(frameNum);
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

        // Calculations Per Frame
        calcsPerFrame = (numCalculations / (frameNum - oldFrameNum));

        lastFPStime = currentFPStime;

        oldFrameNum = frameNum;
        numCalculations = 0;
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
   
    if(wire) {
        //glDisable(GL_LIGHTING);
        renderClothStructure();

    } else {

        // Renders 3D Objects 
        glEnable(GL_TEXTURE_2D);
        renderCloth(); 

        glDisable(GL_TEXTURE_2D);
    }



    // Draw Shapes:
    for(int i = 0; i < numShapes; i++) {
        glCallList(shapeDrawLists[i]);
    }
    
    // Sets OpenGL Variables to Render 2D:
    glut2DSetup();
    //printHUD();
    printOptionsHeader();

    if(showOptions) {
        printOptions();
    }

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
        
        // TODO: 
        glutSolidSphere(s->getRadius() - 0.01f, 50 ,50 );
        glPopMatrix();
    } 

    if(s->getType() == "PLANE") {



        glm::vec3 norm = s->getNormal();
        glm::vec3 point = s->getUL();

        glPushMatrix();

        glBegin(GL_QUADS);
            glColor3f(0.804f, 0.412f, 0.118f);
            
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
    
    cloth = new Cloth(density, corners[0], corners[1], corners[2], corners[3], euler);

    if(debugStats) {
        cloth->printStats();
    }

    //cloth = new Cloth(width, height, corners[0], corners[1], corners[2], corners[3], euler);
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

        // Performance Modifying Keys
        case 'r':           // Toggles if program is running
            running = !running;
            break;

        case 'c':           // Switches between Constant FPS & TimeStep
            constantStep = !constantStep;
            break;

        case 't':           // Steps through numTimeStep Calculations
            if(!running) {
                stepFrame();
            }
            break;

        case 'y':           // Increases Timestep Value
            timestep += STEP_INC;
            break;

        case 'u':           // Decreases Timestep Value
            if(timestep >= 0.002f) {
                timestep -= STEP_INC;
            }
            break;

        case 'q':           // Reset's the Cloth to initial position
            cloth = NULL;
            loadCloth(inputFile);
            frameNum = 0;
            break;
        
        // Force Modifying Keys
        case 'g':           // Toggle Gravity
            gravity = !gravity;
            std::cout << "Gravity is now ";
            if(gravity) {
                std::cout << "ON" << std::endl;
            } else {
                std::cout << "OFF" << std::endl;
            }
            break;

        case 'f':           // Toggle Wind
            wind = !wind;
            break;

        case 'j':           // Increment Wind Force
            windForce = glm::normalize(windForce) * (glm::length(windForce) + windINC);
            break;

        case 'k':           // Decrement Wind Force
            windForce = glm::normalize(windForce) * (glm::length(windForce) - windINC);
            break;

        // Shading, Drawing and Texture Modifying Keys
        case 's':           // Toggle Smooth Shading
            smooth = !smooth;
            if(smooth) {
                glShadeModel(GL_SMOOTH);
            } else {
                glShadeModel(GL_FLAT);
            }
            break;

        case 'l':           // Toggle Lighting
            light = !light;
            if(light) {
                std::cout << "Setting Light On" << std::endl;
               // glEnable(GL_LIGHTING);
            } else {
               // glDisable(GL_LIGHTING);
            }
            break;

        case 'w':           // Toggle Wireframe Mode
            wire = !wire;
            /*
            if(wire) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }*/
            break;

        case '1':   // Checkerboard Texture
            glBindTexture(GL_TEXTURE_2D, texture0);
            currentTexture = "CHECKERBOARD";
            break;

        case '2':   // American Flag Texture
            glBindTexture(GL_TEXTURE_2D, texture1);
            currentTexture = "AMERICAN_FLAG";
            break;

        case '3':   // American Flag Texture
            glBindTexture(GL_TEXTURE_2D, texture2);
            currentTexture = "GRADIENT";
            break;

        case 'o':           // Toggles whether the Options List is shown
            if(viewport.w < 700) {
                showOptions = false;
                reopenOptions = true;
            } else {
                if(!saveImage) {
                    showOptions = !showOptions;
                }
            }
            break;

        // Orientation Modifying Keys
        case '+':
            zTranslate += Z_TRANSLATE_INC;
            break;

        case '-':
            zTranslate -= Z_TRANSLATE_INC;
            break;

        // Exits the Program
        case 'p':
            saveImage = !saveImage;
            showOptions = false;
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

/*
void screendump(int W, int H) {
    FILE   *out = fopen("screenshot.tga","wb");
    char   *pixel_data = new char[3*W*H];
    short  TGAhead[] = { 0, 2, 0, 0, 0, 0, W, H, 24 };
     
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, W, H, GL_BGR, GL_UNSIGNED_BYTE, pixel_data);
     
    fwrite(&amp; TGAhead, sizeof(TGAhead), 1, out);
    fwrite(pixel_data, 3*W*H, 1, out);
    fclose(out);
 
    delete[] pixel_data; 
}*/

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



