#include "pch.h"
#include "LogisticsSolver.h"

LogisticsSolver::LogisticsSolver(vector<LogisticsNode> nodes,
                                 LogisticsCostModel    costModel,
                                 OptimizationWeights   weights)
    : m_nodes(move(nodes)),
      m_costModel(costModel),
      m_weights(weights)
{
    m_weights.Normalize();
}

// 加权重心：以年货运需求量为权重计算所有节点的空间重心
MPoint LogisticsSolver::ComputeWeightedCentroid() const
{
    MVector acc(0.0, 0.0, 0.0);
    double  totalWeight = 0.0;

    for (const auto& node : m_nodes) {
        MPoint pt = node.ToMPoint();
        vector<double> xyz = pt.Get_XYZ();
        double w = node.annualDemandTons;
        if (w <= 0.0) w = 1.0;
        acc += MVector(xyz[0], xyz[1], xyz[2]) * w;
        totalWeight += w;
    }

    if (totalWeight > 0.0) acc /= totalWeight;
    acc.Normalize();

    MEarth earth(acc);
    return earth.ToEarthSurface();
}

// 通用梯度下降（四方向探索，自适应步长）
MPoint LogisticsSolver::GradientDescent(
    const function<double(const MPoint&)>& objective,
    MPoint initialPoint,
    double initialStep,
    int    maxIter,
    double tolerance) const
{
    MPoint focus = initialPoint;
    double level = initialStep;
    int    iter  = 0;

    while (iter < maxIter) {
        iter++;

        MPoint E = focus; E.Move( level,  0.0);
        MPoint W = focus; W.Move(-level,  0.0);
        MPoint N = focus; N.Move( 0.0,    level);
        MPoint S = focus; S.Move( 0.0,   -level);

        double base  = objective(focus);
        double east  = objective(E);
        double west  = objective(W);
        double north = objective(N);
        double south = objective(S);

        double minVal = base;
        MPoint best   = focus;
        if (east  < minVal) { minVal = east;  best = E; }
        if (west  < minVal) { minVal = west;  best = W; }
        if (north < minVal) { minVal = north; best = N; }
        if (south < minVal) { minVal = south; best = S; }

        if (minVal >= base) {
            level /= 2.0;
            if (level < tolerance) break;
            continue;
        }
        if (fabs(minVal - base) > 16.0) {
            level *= 2.0;
        }
        focus = best;
    }
    return focus;
}

// ── 目标函数实现 ───────────────────────────────────────────────

// Warehouse points returned by ToEarthSurface() store L/B in radians
// (atan2 result). Convert to degrees before calling CalcHaversineKm.
static inline double NodeDistKm(const LogisticsNode& node, const MPoint& warehousePt)
{
    double wLon = warehousePt.GetLongitude() * RAD_TO_DEG;
    double wLat = warehousePt.GetLatitude()  * RAD_TO_DEG;
    return CalcHaversineKm(node.longitude, node.latitude, wLon, wLat);
}

// 加权等距方差：Σ w_i*(d_i - d̄_w)² / Σ w_i
double LogisticsSolver::EvaluateWeightedVariance(const MPoint& p) const
{
    double totalW   = 0.0;
    double wAvgDist = 0.0;

    for (const auto& node : m_nodes) {
        double w    = node.annualDemandTons;
        if (w <= 0.0) w = 1.0;
        wAvgDist += w * NodeDistKm(node, p);
        totalW   += w;
    }
    if (totalW > 0.0) wAvgDist /= totalW;

    double variance = 0.0;
    for (const auto& node : m_nodes) {
        double w    = node.annualDemandTons;
        if (w <= 0.0) w = 1.0;
        double diff = NodeDistKm(node, p) - wAvgDist;
        variance   += w * diff * diff;
    }
    return (totalW > 0.0) ? variance / totalW : variance;
}

// 加权年运输成本（万元）
double LogisticsSolver::EvaluateTransportCost(const MPoint& p) const
{
    double total = 0.0;
    for (const auto& node : m_nodes) {
        double dist = NodeDistKm(node, p);
        total += m_costModel.CalcTransportCost(dist, node.annualDemandTons, node.costRatePerTonKm);
    }
    return total / 10000.0; // 转换为万元
}

// 时间成本总和（万元）
double LogisticsSolver::EvaluateTimeCost(const MPoint& p) const
{
    double total = 0.0;
    for (const auto& node : m_nodes) {
        double dist       = NodeDistKm(node, p);
        double annualFreq = node.deliveryFreqPerMonth * 12.0;
        total += m_costModel.CalcTimeCost(dist, annualFreq);
    }
    return total / 10000.0; // 转换为万元
}

// 多目标评分（加权归一化，越小越好）
// 各分量先除以参考值归一化，再按权重叠加
double LogisticsSolver::EvaluateMultiObj(const MPoint& p) const
{
    double varScore  = EvaluateWeightedVariance(p);
    double costScore = EvaluateTransportCost(p);
    double timeScore = EvaluateTimeCost(p);

    // 参考归一化因子（粗略量级，使各项可比）
    const double varRef  = 1.0e6;   // 方差参考（平方公里）
    const double costRef = 5000.0;  // 运输成本参考（万元）
    const double timeRef = 500.0;   // 时间成本参考（万元）

    double normVar  = varScore  / varRef;
    double normCost = costScore / costRef;
    double normTime = timeScore / timeRef;

    return m_weights.distanceWeight * normVar
         + m_weights.costWeight     * normCost
         + m_weights.timeWeight     * normTime;
}

// 约束检查：候选点到每个节点的距离不超过该节点的最大服务半径
bool LogisticsSolver::CheckServiceRadius(const MPoint& p, double globalMaxKm) const
{
    for (const auto& node : m_nodes) {
        double dist  = NodeDistKm(node, p);
        double limit = (globalMaxKm > 0.0) ? globalMaxKm : node.maxServiceRadiusKm;
        if (dist > limit) return false;
    }
    return true;
}

// ── 求解器接口 ────────────────────────────────────────────────

// 加权等距点（以货运需求量为权重，最小化加权距离方差）
MPoint LogisticsSolver::SolveWeighted()
{
    MPoint initial = ComputeWeightedCentroid();
    return GradientDescent(
        [this](const MPoint& p) { return EvaluateWeightedVariance(p); },
        initial);
}

// 多目标优化（同时优化等距性、运输成本、时间成本）
MPoint LogisticsSolver::SolveMultiObj()
{
    MPoint initial = ComputeWeightedCentroid();
    return GradientDescent(
        [this](const MPoint& p) { return EvaluateMultiObj(p); },
        initial);
}

// 带服务半径约束的求解（在满足半径约束的前提下最优化）
MPoint LogisticsSolver::SolveConstrained(double globalMaxRadiusKm)
{
    MPoint initial = ComputeWeightedCentroid();

    // 带软约束的目标函数：违反约束时加大惩罚项
    auto constrainedObj = [this, globalMaxRadiusKm](const MPoint& p) -> double {
        double baseScore = EvaluateMultiObj(p);
        double penalty   = 0.0;

        for (const auto& node : m_nodes) {
            double dist  = NodeDistKm(node, p);
            double limit = (globalMaxRadiusKm > 0.0) ? globalMaxRadiusKm : node.maxServiceRadiusKm;
            if (dist > limit) {
                double excess = dist - limit;
                penalty += excess * excess / (limit * limit);
            }
        }
        return baseScore + penalty * 10.0;
    };

    return GradientDescent(constrainedObj, initial);
}
