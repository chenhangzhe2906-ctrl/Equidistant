#include "pch.h"
#include "Coordinate.h"
#include "MPoint.h"
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

// 将初始点重置为加权重心（按需求权重加权）
void Coordinate::ResetToWeightedCentroid()
{
	MVector d_vec(0.0, 0.0, 0.0);
	double totalWeight = 0.0;
	for (int i = 0; i < length; i++) {
		double w = CityList[i].GetDemandWeight();
		if (w <= 0.0) w = 1.0;
		MVector weighted = Pos[i] * w;
		d_vec += weighted;
		totalWeight += w;
	}
	if (totalWeight > 0.0) d_vec /= totalWeight;
	d_vec.Normalize();
	MEarth t_earth(d_vec);
	Focus = t_earth.ToEarthSurface();
}

// 原始等距方差（无权重）
double Coordinate::CalculateVariance(const MPoint &f) const
{
	double average = 0.0;
	double Variance = 0.0;
	for (auto& it : CityList) {
		average += it.DistanceTo(f) / length;
	}
	for (auto& it : CityList) {
		Variance += (it.DistanceTo(f) - average) * (it.DistanceTo(f) - average);
	}
	return Variance;
}

// 加权等距方差：Σ w_i*(d_i - d̄)² / Σ w_i，d̄ 为加权平均距离
double Coordinate::CalculateWeightedVariance(const MPoint& f) const
{
	double totalWeight = 0.0;
	double weightedAvgDist = 0.0;
	double weightedVariance = 0.0;

	for (auto& it : CityList) {
		double w = it.GetDemandWeight();
		if (w <= 0.0) w = 1.0;
		weightedAvgDist += w * it.DistanceTo(f);
		totalWeight += w;
	}
	if (totalWeight > 0.0) {
		weightedAvgDist /= totalWeight;
	}

	for (auto& it : CityList) {
		double w = it.GetDemandWeight();
		if (w <= 0.0) w = 1.0;
		double diff = it.DistanceTo(f) - weightedAvgDist;
		weightedVariance += w * diff * diff;
	}
	return (totalWeight > 0.0) ? weightedVariance / totalWeight : weightedVariance;
}

// 多目标评分：加权运输成本 + 加权等距方差
// 运输成本 = Σ w_i * d_i * cm_i；时间成本 = Σ w_i * d_i * ct_i
// 最终分数 = 0.5 * 归一化运输成本 + 0.3 * 加权方差 + 0.2 * 归一化时间成本
double Coordinate::CalculateScore(const MPoint& f) const
{
	double totalWeight = 0.0;
	double transportCost = 0.0;
	double timeCost = 0.0;

	for (auto& it : CityList) {
		double w  = it.GetDemandWeight();
		if (w <= 0.0) w = 1.0;
		double d  = it.DistanceTo(f);
		double cm = it.GetCostMoney();
		double ct = it.GetCostTime();
		transportCost += w * d * cm;
		timeCost      += w * d * ct;
		totalWeight   += w;
	}
	if (totalWeight > 0.0) {
		transportCost /= totalWeight;
		timeCost      /= totalWeight;
	}

	double weightedVar = CalculateWeightedVariance(f);

	return 0.5 * transportCost + 0.3 * weightedVar + 0.2 * timeCost;
}

// 原始等距求解器（梯度下降，最小化等距方差）
MPoint Coordinate::simpleSolver()
{
	int iterations = 0;
	double level = 0.0001;
	while (CalculateVariance(Focus) > 4.0 || iterations < 1000000) {
		iterations += 1;
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

// 多目标物流求解器（加权重心初始化 + 基于 CalculateScore 的梯度下降）
MPoint Coordinate::advancedSolver()
{
	ResetToWeightedCentroid();

	int iterations = 0;
	double level = 0.0001;
	double currentScore = CalculateScore(Focus);

	while (currentScore > 1.0 || iterations < 1000000) {
		iterations++;

		MPoint Eastward  = Focus; Eastward.Move( level,  0.0);
		MPoint Westward  = Focus; Westward.Move(-level,  0.0);
		MPoint Northward = Focus; Northward.Move(0.0,    level);
		MPoint Southward = Focus; Southward.Move(0.0,   -level);

		double east  = CalculateScore(Eastward);
		double west  = CalculateScore(Westward);
		double north = CalculateScore(Northward);
		double south = CalculateScore(Southward);

		double minScore = currentScore;
		MPoint bestPoint = Focus;
		if (east  < minScore) { minScore = east;  bestPoint = Eastward;  }
		if (west  < minScore) { minScore = west;  bestPoint = Westward;  }
		if (north < minScore) { minScore = north; bestPoint = Northward; }
		if (south < minScore) { minScore = south; bestPoint = Southward; }

		if (minScore >= currentScore) {
			level /= 2.0;
			if (level < 1e-10) break;
			continue;
		}
		if (fabs(minScore - currentScore) > 16.0) {
			level *= 2.0;
		}
		Focus = bestPoint;
		currentScore = minScore;
	}
	return Focus;
}

