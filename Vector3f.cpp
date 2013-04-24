#include "Vector3f.h"

// Constructors
Vector3f::Vector3f(float x, float y, float z) 
 x m_x(x), m_y(y), m_z(z)
{
}

Vector3f::Vector3f(const Vector3f& vector) 
 : m_x(vector.getX()), m_y(vector.getY()), m_z(vector.getZ())
{
}

Vector3f::Vector3f() 
 : m_x(0), m_y(0), m_z(0)
{
}

// Calculate the length of the vector
float Vector3f::length() const{
	return sqrt(m_x*m_x+m_y*m_y+m_z*m_z);
}

// Calculate the squared length of the vector
float Vector3f::lengthSquared() const{
	return m_x*m_x+m_y*m_y+m_z*m_z;
}

// Create a unit length vector in this vector's direction
Vector3f Vector3f::normalized() const{
	float l = length();
	return Vector3f(m_x / l, m_y/l, m_z/l);
}

// Add a vector to this one
void Vector3f::operator += (const Vector3f &vector){
	m_x += vector.getX();
	m_y += vector.getY();
	m_z += vector.getZ();
}


// Multiply this vector by a scalar amount
void Vector3f::operator*= (const float &scalar){
	m_x*=scalar; 
	m_y*=scalar; 
	m_z*=scalar;
}


// Add two vectors
Vector3f Vector3f::operator+ (const Vector3f &vector) const{
	return Vector3f(m_x+vector.getX(), m_y+vector.getY(), m_z+vector.getZ());
}

// Multiply a vector by a scalar amount
Vector3f Vector3f::operator* (const float &scalar) const{
	return Vector3f(m_x*scalar, m_y*scalar, m_z*scalar);
}

// Find the cross product of this vector and another
Vector3f Vector3f::cross(const Vector3f &vector) const{
	return Vector3f(m_y*vector.getZ() - m_z*vector.getY(), m_z*vector.getX() - m_x*vector.getZ(), m_x*vector.getY() - m_y*vector.getX());
}

// Find the dot product of this vector and another
float Vector3f::dot(const Vector3f &vector) const{
	return m_x*vector.getX() + m_y*vector.getY() + m_z*vector.getZ();
}