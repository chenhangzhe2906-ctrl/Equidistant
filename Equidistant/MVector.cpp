#include "pch.h"
#include "MVector.h"

MVector::MVector(void)
{
	Set(0.0, 0.0, 0.0);
	return;
}

MVector::MVector(double x, double y, double z)
{
	Set(x, y, z);
	return;
}

MVector::~MVector(void){  
}

inline void MVector::Set(double x, double y, double z)  
{
	m_coord[0] = x;
	m_coord[1] = y;
	m_coord[2] = z;
	return;
}

MVector& MVector::operator=(const MVector& vec) 
{
	for (int i = 0; i < 3; i++) {
		this->m_coord[i] = vec.m_coord[i];
	}
	return *this;
}

double MVector::operator[](int idx) const 
{
	return m_coord[idx];
}

double MVector::Magnitude() const  
{
	return sqrt(m_coord[0] * m_coord[0] + m_coord[1] * m_coord[1] + m_coord[2] * m_coord[2]);
}

double MVector::Length2() const  
{
	return m_coord[0] * m_coord[0] + m_coord[1] * m_coord[1] + m_coord[2] * m_coord[2];
}

void MVector::Normalize()  
{
	for (int i = 0; i < 3; i++) {
		m_coord[i] /= this->Magnitude();
	}
	return;
}

bool MVector::IsZero() const  
{
	if (m_coord[0] == 0.0 && m_coord[1] == 0.0 && m_coord[2] == 0.0) {
		return true;
	}
	return false;
}

