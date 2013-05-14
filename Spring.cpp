#include <iostream>
#include <math.h>
#include <vector>
#include "glm/glm.hpp"

#include "Spring.h"
#include "Vertex.h"


//****************************************************
// Spring Class - Constants
//****************************************************
const float UNIT_SPRING = 100.0f;
int stretchCount = 0;
int shearCount = 0;
int bendCount = 0;

// Class Variables
int stretchConst;
int shearConst;
int bendConst;

// Distance Constraint Variables
float DEF_TOLERANCE = 0.01f;


//****************************************************
// Spring Class - Constructors
//****************************************************
Spring::Spring(Vertex* v1, Vertex* v2, std::string t) {
	vertex1 = v1;
	vertex2 = v2;

	type = t;

	tolerance = DEF_TOLERANCE;

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

void Spring::setTolerance(float newTolerance) {
	tolerance = newTolerance;
}

void Spring::setSpringConstant(float newUnitSpring){
	unitConstant = newUnitSpring;
	springConstant = unitConstant / restDistance;
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


void Spring::lengthConstraint() {

	glm::vec3 v1 = vertex1->getPos();
	glm::vec3 v2 = vertex2->getPos();

	glm::vec3 springVec = v2-v1;

	float length = glm::length(springVec);

	float upperBound = restDistance * (1.0f + tolerance);
	float lowerBound = restDistance * (1.0f - tolerance);

	// If Longer Than Tolerance
	if(length > upperBound) {

		glm::vec3 correction = springVec * (1.0f - upperBound/length)*0.5f;

		vertex1->offsetCorrection(correction);
		vertex2->offsetCorrection(-correction);

	}

	if(length < lowerBound) {
		glm::vec3 correction = springVec * (1.0f - lowerBound/length)*0.5f;

		vertex1->offsetCorrection(correction);
		vertex2->offsetCorrection(-correction);

	}
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
