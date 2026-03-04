#pragma once
#include "pch.h"
#include "MPoint.h"
#include "MVector.h"

class MEllipse {
public:
	MEllipse(double a, double b);
	virtual ~MEllipse();
	//MPoint GetCenter() const;
	double GetHalfWidth() const;
	double GetHalfHeight() const;
	double GetPerimeter() const;
	
protected:
	double m_a, m_b;
};