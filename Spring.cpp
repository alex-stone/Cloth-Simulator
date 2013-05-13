#include <iostream>
#include <math.h>
#include <vector>
#include "glm/glm.hpp"

#include "Spring.h"
#include "Vertex.h"

//****************************************************
// Spring Class - Constructors
//****************************************************
Spring::Spring(Vertex* v1, Vertex* v2, std::string t) {
	vertex1 = v1;
	vertex2 = v2;

	restDistance = glm::length(v2-v1);
	springConstant = 1;

	type = t;
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

	float displacement = glm::length(springVec);
	float magnitude = springConstant * displacement;

	vertex1->addForce(springVec * magnitude);
	vertex2->addForce(springVec * magnitude * (-1.0f));

}

