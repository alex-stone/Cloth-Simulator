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
  	float springConstant;
  	float restDistance;

  	// Type, "STRETCH", "SHEAR", "BEND"
  	std::string type;

  public:
  	// Constructors
    Spring(Vertex* v1, Vertex* v2, std::string t);
  	Spring(Vertex* v1, Vertex* v2, float kconstant, std::string t);

  	// Getters
  	std::string getType() { return type; };

  	// Force Due to Spring:
  	glm::vec3 getForce();
  	void applyForce();

};
#endif
