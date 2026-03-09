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
	void extraInit(double dW, double cM, double cT);
	vector <double>  Get_XYZ() const;
	vector <double>  Get_XYZ(double L, double B);
	vector <double>  Get_LB() const;
	vector <double>  Get_LB(double x, double y, double z);
	void Move(double m_L, double m_B);
	double DistanceTo(const MPoint& other) const;
	MPoint& operator=(const MPoint& point);

	// 物流属性访问器
	double GetLongitude()    const { return L; }
	double GetLatitude()     const { return B; }
	double GetDemandWeight() const { return demandWeight; }
	double GetCostMoney()    const { return costMoney; }
	double GetCostTime()     const { return costTime; }

private:
	double X;
	double Y;
	double Z;
	double L;  // 经度，单位为度
	double B;  // 纬度，正纬为正
	double N;
	double demandWeight; // 需求权重 0-1
	double costMoney;    // 运输成本系数（元/公里）
	double costTime;     // 时间成本系数（元/小时）
};
