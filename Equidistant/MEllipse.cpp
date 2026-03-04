#include"pch.h"
#include "MEllipse.h"

MEllipse::MEllipse(double a, double b)
{
	m_a = a;
	m_b = b;
	return;
}

MEllipse::~MEllipse()
{
}

double MEllipse::GetHalfWidth() const
{
	return m_a;
}

double MEllipse::GetHalfHeight() const
{
	return m_b;
}

double MEllipse::GetPerimeter() const
{
	return 2 * PI * m_b + 4 * (m_a - m_b);
}



