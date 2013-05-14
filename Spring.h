#ifndef SPRING_H
#define SPRING_H

#include "glm/glm.hpp"
#include "Vertex.h"

//****************************************************
// Spring Header Definition
//****************************************************

class Spring {
  private:
  	Vertex* vertex1;
  	Vertex* vertex2;

  	// Spring Information
    float unitConstant;

  	float springConstant;
  	float restDistance;

    float tolerance;


  	// Type, "STRETCH", "SHEAR", "BEND"
  	std::string type;

  public:
  	// Constructors
    Spring(Vertex* v1, Vertex* v2, std::string t);
  	Spring(Vertex* v1, Vertex* v2, float kconstant, std::string t);

  	// Getters
  	std::string getType() { return type; };

    Vertex* getV1() { return vertex1; };
    Vertex* getV2() { return vertex2; };

    glm::vec3 getPos1() { return vertex1->getPos(); };
    glm::vec3 getPos2() { return vertex2->getPos(); };


  	// Force Due to Spring:
  	glm::vec3 getForce();
  	void applyForce();

    void setTolerance(float newTolerance);
    void setSpringConstant(float newSpring);

    // Directly moves position instead of adding force
    void applyCorrection();
    void lengthConstraint();

    // Prints Stats about Springs
    void static printStats();

};
#endif
