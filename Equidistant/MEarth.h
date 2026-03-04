#pragma once
#include "pch.h"
#include "MEllipse.h"


class MEarth:public MEllipse
{
public:
	MEarth(MVector vec);
	~MEarth();
	MPoint ToEarthSurface();

private:
	MVector d_vec;
	double angle;
};

