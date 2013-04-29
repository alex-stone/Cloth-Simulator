#ifndef VECTOR3F_H
#define VECTOR3F_H

#include <math.h>

// A basic 3d vector class
class Vector3f
{
public:
	float m_x, m_y, m_z;
public:
	
	// Constructors
	Vector3f(float x, float y, float z);
	Vector3f(const Vector3f &vector);
	Vector3f();				

	float length() const;						// Calculate the length of the vector
	float lengthSquared() const;				// Calculate the squared length of the vector
	Vector3f normalized() const;				// Create a unit length vector in this vector's direction
	void operator += (const Vector3f &vector);	// Add a vector to this one
	
	void operator *= (const float &scalar);		// Multiply this vector by a scalar amount


	Vector3f operator+ (const Vector3f &vector) const;// Add two vectors
	Vector3f operator* (const float &scalar) const;	// Multiply a vector by a scalar amount

	Vector3f cross(const Vector3f &vector) const;		// Find the cross product of this vector and another
	float dot(const Vector3f &vector) const;			// Find the dot product of this vector and another

	// Accessor functions
	inline float getX() { return m_x; }
	inline float getY() { return m_y; }
	inline float getZ() { return m_z; }
	const float* getXLocation() const { return &m_x; }
	inline void setX(const float x) { m_x = x; }
	inline void setY(const float y) {m_y = y; }
	inline void setZ(const float z) { m_z = z; }
};

#endif