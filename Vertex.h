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

    // TODO: Spring Connections:
    // 4 Stretch Spring: Left, Up, Right, Down
    Vertex** stretch;
    float stretchRestDist;

    // 4 Shear Springs: Up-Left, Up-Right, Down-Right, Down-Left
    Vertex** shear;
    float shearRestDist;
    
    // 4 Bend Springs: Left, Up, Right, Down
    Vertex** bend;
    float bendRestDist;

    // TODO: Variables
    float lastTimeUpdated;
    bool fixed;
    float mass;

    bool euler;

    // Spring Constants - Stretch, Shear, Bend
    float stretchConstant;
    float shearConstant;
    float bendConstant;

    float dampConstant;

    // Function Initializations:
    void initSpringToNull();
    void initPhysicalProps(float a, float b, float c); 
    void initSpringConstants(float stretchConst, float shearConst, float bendConst, float dampConstant);

  public:
    // Constructors:
    Vertex();
    Vertex(float a, float b, float c);
    Vertex(float a, float b, float c, bool isFixed);
    Vertex(float a, float b, float c, float stretchConst, float shearConst, float bendConst);
    Vertex(float a, float b, float c, float stretchConst, float shearConst, float bendConst, bool isFixed);

    // Getters
    glm::vec3 getPos() { return position; };
    glm::vec3 getVelocity() { return velocity; };
    glm::vec3 getAccel() { return acceleration; };
    glm::vec3 getNorm() { return glm::normalize(normal); };

    void setPosition(int x, int y);
    void setSpringRestLengths(float stretch, float bend, float shear);
    void setFixedVertex(bool isFixed);
    void setNormal(glm::vec3 newNorm);

    void resetNorm();

    // Connect Vertex a, to this Vertex, in direction n
    void connectStretch(Vertex* a, int n);
    void connectShear(Vertex* a , int n);
    void connectBend(Vertex* a, int n);

    // Force Calculation Functions:
    void updateVerlet(float timeChange);
    void updateEuler(float timeChange);
    void update(float timeChange, bool euler);

    void updateAccel(glm::vec3 externalForces);
    void updateNormal(glm::vec3 addNorm);
    glm::vec3 findNormal(Vertex* v2, Vertex* v3);
    
    void updateAfterCollide(glm::vec3 newPos, glm::vec3 newVel);
   // void updateCollisions(glm::vec3 &c, float radius);
    
    glm::vec3 getSpringForce();
    glm::vec3 getDampForce();
    glm::vec3 getForceFromSpring(float restLength, float stretchConstant, glm::vec3 springVec);

    // Direction To Vertex:
    glm::vec3 vectorTo(Vertex* a);


    // Force Calculation Functions
    //      External Forces:
    //          - Gravity, Wind, etc...
    //      Internal Forces:
    //          - 12 Spring Connections

    void printPosition();
    void printVelocity();
    void printAccel();

};

#endif
