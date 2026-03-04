#include "pch.h"
#include "Coordinate.h"
#include"MPoint.h"
#include "MVector.h"
#include "MEllipse.h"
#include "MEarth.h"

Coordinate::Coordinate(vector <MPoint> Cities):CityList(Cities)
{
	length = CityList.size();
	MVector* d_vec = new MVector(0,0,0);
	for (auto& it : CityList) {
		Pos.push_back(MVector(it.Get_XYZ()[0], it.Get_XYZ()[1], it.Get_XYZ()[2]));
		*d_vec += Pos.back();
	}
	d_vec->Normalize();
	MEarth* t_earth = new MEarth(*d_vec);
	Focus = t_earth->ToEarthSurface();
	delete d_vec;
	delete t_earth;
	return;
}

double Coordinate::CalculateVariance(MPoint &f)
{
	double average = 0.0;
	double Variance = 0.0;
	for (auto& it : CityList) {
		average += it.DistanceTo(f)/length;
	}
	for (auto& it : CityList) {
		Variance += (it.DistanceTo(f) - average) * (it.DistanceTo(f) - average);
	}
	return Variance;
}

MPoint Coordinate::Solver()
{
	int iterations = 0;
	double level=0.0001;
	while (CalculateVariance(Focus) > 4.0 || iterations < 1000000) {
		iterations += 1;
		//MPoint TempFocus = Focus;
		MPoint Eastward = Focus;
		Eastward.Move(level, 0);
		double east = CalculateVariance(Eastward);
		MPoint Westward = Focus;
		Westward.Move(-1*level, 0);
		double west = CalculateVariance(Westward);
		MPoint Northward = Focus;
		Northward.Move(0.0, level);
		double north = CalculateVariance(Northward);
		MPoint Southward = Focus;
		Southward.Move(0.0, -1*level);
		double south = CalculateVariance(Southward);
		double minVariance = CalculateVariance(Focus);
		MPoint bestPoint = Focus; 
		if (east < minVariance) {
			minVariance = east;
			bestPoint = Eastward;
		}
		if (west < minVariance) {
			minVariance = west;
			bestPoint = Westward;
		}
		if (north < minVariance) {
			minVariance = north;
			bestPoint = Northward;
		}
		if (south < minVariance) {
			minVariance = south;
			bestPoint = Southward;
		}
		if (minVariance == CalculateVariance(Focus)) {
			level = level / 2;
			continue;
		}
		if (fabs(minVariance - CalculateVariance(Focus) > 16)) {
			level=level*2;
		}
		Focus = bestPoint;
	}
	return Focus;
}
