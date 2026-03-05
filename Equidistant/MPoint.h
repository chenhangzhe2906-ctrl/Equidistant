#pragma once
#include "pch.h"

class MPoint {
public:
	MPoint();
	~MPoint();
	MPoint(double L, double B);
	MPoint(double L, double B, double dW, double cM, double cT);
	MPoint(double x, double y, double z);
	MPoint(double x, double y, double z, double dW, double cM, double cT);
	void Set(double x, double y, double z);
	void extraInit(double dW,double cM, double cT);
	vector <double>  Get_XYZ() const;
	vector <double>  Get_XYZ(double L, double B);
	vector <double>  Get_LB() const;
	vector <double>  Get_LB(double x, double y, double z);
	void Move(double m_L, double m_B);
	double DistanceTo(const MPoint& other)const;
	MPoint& operator=(const MPoint& point);

private:
	double X;
	double Y;
	double Z;
	double L; // 经度，东经为正
	double B;  // 纬度，北纬为正
	double N;
	double demandWeight;//权重0-1
	double costMoney;// 土地人力成本评分
	double costTime;// 交通便利性评分
};
