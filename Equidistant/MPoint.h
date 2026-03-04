#pragma once
#include "pch.h"

class MPoint {
public:
	MPoint();
	~MPoint();
	MPoint(double L, double B);
	MPoint(double x, double y, double z);
	void Set(double x, double y, double z);
	vector <double>  Get_XYZ() const;
	vector <double>  Get_XYZ(double L, double B);
	vector <double>  Get_LB() const;
	vector <double>  Get_LB(double x, double y, double z);
	void Move(double m_L, double m_B);
	double DistanceTo(MPoint& other)const;
	MPoint& operator=(const MPoint& point);

private:
	double X;
	double Y;
	double Z;
	double L; // 经度，东经为正
	double B;  // 纬度，北纬为正
	double N;
};
