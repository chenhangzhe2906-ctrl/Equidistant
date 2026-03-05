#pragma once
#include "pch.h"
#include"MPoint.h"
#include"MEarth.h"
#include "MEllipse.h"
#include"MVector.h"

class MPoint;
class MVector;

class Coordinate
{
public:
	Coordinate(vector <MPoint> Cities);

public:
	double CalculateVariance (const MPoint& f) const;
	double CalculateScore(const MPoint& f);
	MPoint simpleSolver();
	MPoint advancedSolver();

private:
	vector <MPoint> CityList;
	vector <MVector> Pos;
	MPoint Focus;
	int length;
};

  