#ifndef VERTEX_H
#define VERTEX_H

#include "glm/glm.hpp"

//****************************************************
// Vertex Header Definition
//****************************************************

class Vertex {
  private:
    // TODO: Vertex Properties:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 normal;

    // Cloth Coordinate Position;
    int xPos;
    int yPos;

    // Verlet Integration Properties:
    glm::vec3 oldPos;
    float oldTimeChange;
    bool firstUpdate;

    // TODO: Variables
    float lastTimeUpdated;
    bool fixed;
    float mass;

    bool euler;


    float dampConstant;

    // Function Initializations:
  //  void initSpringToNull();
    void initPhysicalProps(float a, float b, float c); 
   // void initSpringConstants(float stretchConst, float shearConst, float bendConst, float dampConstant);

  public:
    // Constructors:
    Vertex();
    Vertex(float a, float b, float c);
    Vertex(float a, float b, float c, bool isFixed);
   // Vertex(float a, float b, float c, float stretchConst, float shearConst, float bendConst);
   // Vertex(float a, float b, float c, float stretchConst, float shearConst, float bendConst, bool isFixed);

    // Getters
    glm::vec3 getPos() { return position; };
    glm::vec3 getVelocity() { return velocity; };
    glm::vec3 getAccel() { return acceleration; };
    glm::vec3 getNorm() { return glm::normalize(normal); };

    int getXPos() { return xPos; };
    int getYPos() { return yPos; };


    void setPosition(int x, int y);
    //void setSpringRestLengths(float stretch, float bend, float shear);
    void setFixedVertex(bool isFixed);

    // Updates Position based on Acceleration & Timestep
    void update(float timeChange, bool euler);
    void updateVerlet(float timeChange);
    void updateEuler(float timeChange);


    void offsetCorrection(glm::vec3 correctionVec);


    // Connect Vertex a, to this Vertex, in direction n
    void connectStretch(Vertex* a, int n);
    void connectShear(Vertex* a , int n);
    void connectBend(Vertex* a, int n);


    void resetAccel();

    // Add Constant Acceleration's i.e. Gravity
    void addAccel(glm::vec3 accel);
    void addForce(glm::vec3 force);

    void updateInternal();

    // Manage Normals
    void updateNormal(glm::vec3 addNorm);
    glm::vec3 findNormal(Vertex* v2, Vertex* v3);

    void setNormal();
    void setNormal(glm::vec3 newNorm);
    void resetNorm();

    
    void updateAfterCollide(glm::vec3 newPos, glm::vec3 newVel);
   // void updateCollisions(glm::vec3 &c, float radius);
    
   // glm::vec3 getSpringForce();
    glm::vec3 getDampForce();
    //glm::vec3 getForceFromSpring(float restLength, float stretchConstant, Vertex* v2);//glm::vec3 springVec);


    // Vertex Helper Functions:

    // Direction To Vertex:
    glm::vec3 vectorTo(Vertex* a);

    // Print Functions
    void printCoordinate();
    void printPosition();
    void printVelocity();
    void printAccel();

};

#endif
