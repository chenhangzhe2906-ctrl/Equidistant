#pragma once
#include "pch.h"
#include "MPoint.h"
#include "MEarth.h"
#include "MEllipse.h"
#include "MVector.h"

class MPoint;
class MVector;

class Coordinate
{
public:
	Coordinate(vector <MPoint> Cities);

public:
	double CalculateVariance(const MPoint& f) const;
	double CalculateWeightedVariance(const MPoint& f) const;
	double CalculateScore(const MPoint& f) const;
	MPoint simpleSolver();
	MPoint advancedSolver();

private:
	void ResetToWeightedCentroid();

	vector <MPoint> CityList;
	vector <MVector> Pos;
	MPoint Focus;
	int length;
};
