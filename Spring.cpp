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
int vertexCount = 0;
int errorCount = 0;
int stretchError = 0;
int shearError = 0;
int bendError = 0;

//****************************************************
// Spring Class - Constructors
//****************************************************
Spring::Spring(Vertex* v1, Vertex* v2, std::string t) {
	vertex1 = v1;
	vertex2 = v2;

	restDistance = glm::length(v2->getPos() - v1->getPos());


	if(restDistance < 0.0f) {
		if(false) {
			std::cout << std::endl;
			std::cout << "Rest Distance < 0 Between Vertices:" << std::endl;
			std::cout << "Vertex1: ";
			v1->printCoordinate();
			v1->printPosition();

			std::cout << "Vertex2: ";
			v2->printCoordinate();
			v2->printPosition();

		}

		if(t == "STRETCH") {
			stretchError++;
		}
		if(t == "SHEAR") {
			shearError++;

			std::cout << std::endl;
			std::cout << "Rest Distance < 0 Between Vertices:" << std::endl;


			std::cout << "Vertex1: ";
			v1->printCoordinate();
			v1->printPosition();

			std::cout << "Vertex2: ";
			v2->printCoordinate();
			v2->printPosition();
		}
		if(t == "BEND") {
			bendError++;
		}

		errorCount++;
	}

	vertexCount++;

	springConstant = UNIT_SPRING / restDistance;

	/*
	if(springConstant < 0.0f) {
		std::cout << "Spring Constant < 0" << std::endl;
	}*/


	type = t;

	if(vertexCount >= 3352) {
		std::cout << "# of Springs = " << vertexCount << std::endl;
		std::cout << "# of Error Springs = " << errorCount << std::endl;
		std::cout << "# of STRETCH Error Springs = " << stretchError << std::endl;
		std::cout << "# of SHEAR Error Springs = " << shearError<< std::endl;
		std::cout << "# of BEND Error Springs = " << bendError << std::endl;
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

	vertex1->addForce(springVec * magnitude);
	vertex2->addForce(springVec * magnitude * (-1.0f));

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
