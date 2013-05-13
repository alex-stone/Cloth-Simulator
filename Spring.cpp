#include <iostream>
#include <math.h>
#include <vector>
#include "glm/glm.hpp"

#include "Spring.h"
#include "Vertex.h"


//****************************************************
// Spring Class - Constants
//****************************************************
const float UNIT_SPRING = 10.0f;
int stretchCount = 0;
int shearCount = 0;
int bendCount = 0;

// Class Variables
int stretchConst;
int shearConst;
int bendConst;

//****************************************************
// Spring Class - Constructors
//****************************************************
Spring::Spring(Vertex* v1, Vertex* v2, std::string t) {
	vertex1 = v1;
	vertex2 = v2;

	type = t;

	restDistance = glm::length(v2->getPos() - v1->getPos());

	// Value Increases with smaller size
	springConstant = UNIT_SPRING / restDistance;


	// Sets the Constants to Class Variables for Display
	if(t == "STRETCH" && stretchCount == 0) {
		stretchConst = springConstant;
		stretchCount++;
	}

	if(t == "SHEAR" && shearCount == 0) {
		shearConst = springConstant;
		shearCount++;
	}

	if(t == "BEND" && bendCount == 0) {
		bendConst = springConstant;
		bendCount++;
	}
}

Spring::Spring(Vertex* v1, Vertex* v2, float kconstant, std::string t) {
	vertex1 = v1;
	vertex2 = v2;

	restDistance = glm::length(v2-v1);
	springConstant = kconstant;

	type = t;
}

//****************************************************
// Spring Class - Magnitude of Force due to Spring
//****************************************************
glm::vec3 Spring::getForce() {
	glm::vec3 v1 = vertex1->getPos();
	glm::vec3 v2 = vertex2->getPos();

	float displacement = glm::length(v2-v1);
	glm::vec3 springVec = vertex1->vectorTo(vertex2);

	return springVec * springConstant * displacement;
}

//****************************************************
// Apply Force:
//		For the given Position of the vertices it
//		Adds the new forces due to springs to the
//		Vertices
//****************************************************
void Spring::applyForce() {
	glm::vec3 v1 = vertex1->getPos();
	glm::vec3 v2 = vertex2->getPos();

	glm::vec3 springVec = v2-v1;

	float displacement = glm::length(springVec) - restDistance;
	float magnitude = springConstant * displacement;

	glm::vec3 dir = glm::normalize(springVec);

	vertex1->addForce(dir * magnitude);
	vertex2->addForce(dir * magnitude * (-1.0f));

	if(vertex1->getXPos() == 2 && vertex1->getYPos() == 2) {
		std::cout << std::endl;
		std::cout << "Type: " << type << std::endl;
		std::cout << "Force Magnitude == " << magnitude << std::endl;
		std::cout << "Force Direction == (" << dir.x << ", " << dir.y << ", " << dir.z << ")" << std::endl;
	}

}

void Spring::applyCorrection() {
	glm::vec3 v1 = vertex1->getPos();
	glm::vec3 v2 = vertex2->getPos();

	glm::vec3 springVec = v2-v1;

	float length = glm::length(springVec);

	glm::vec3 correction = springVec * (1.0f - restDistance/length)*0.5f;

	vertex1->offsetCorrection(correction);
	vertex2->offsetCorrection(-correction);

}

//****************************************************
// Print Stats
//      - Prints Spring Stats:
//****************************************************
void Spring::printStats() {
    std::cout << "---------------------------------------" << std::endl;
    std::cout << " Spring Information: " << std::endl;
    std::cout << "---------------------------------------" << std::endl;    
    std::cout << "Spring constant STRETCH: " << stretchConst << std::endl;
    std::cout << "Spring constant   SHEAR: " << shearConst << std::endl;
    std::cout << "Spring constant    BEND: " << bendConst << std::endl;
}
