#pragma once
#include "pch.h"
#include "MPoint.h"
#include "MEarth.h"
#include "MVector.h"
#include "LogisticsPoint.h"
#include "LogisticsCostModel.h"

// 多目标优化权重配置
struct OptimizationWeights {
    double distanceWeight; // 等距性权重 α（最小化距离方差）
    double costWeight;     // 成本权重   β（最小化运输成本）
    double timeWeight;     // 时效权重   γ（最小化时间成本）

    OptimizationWeights(double alpha = 0.4, double beta = 0.4, double gamma = 0.2)
        : distanceWeight(alpha), costWeight(beta), timeWeight(gamma) {}

    // 验证权重之和是否为1（自动归一化）
    void Normalize() {
        double sum = distanceWeight + costWeight + timeWeight;
        if (sum > 0.0) {
            distanceWeight /= sum;
            costWeight     /= sum;
            timeWeight     /= sum;
        }
    }
};

// 智能物流仓库选址求解器
// 在 Coordinate 层之上封装多目标物流优化逻辑
class LogisticsSolver {
public:
    LogisticsSolver(vector<LogisticsNode> nodes,
                    LogisticsCostModel    costModel,
                    OptimizationWeights   weights = OptimizationWeights());

    // 加权等距点求解：以货运需求量为权重，最小化加权距离方差
    MPoint SolveWeighted();

    // 多目标优化求解：同时优化等距性、运输成本、时间成本
    MPoint SolveMultiObj();

    // 带服务半径约束的求解：确保所有节点在最大服务半径内
    MPoint SolveConstrained(double globalMaxRadiusKm = 0.0);

    // 目标函数评估
    double EvaluateWeightedVariance(const MPoint& p) const;
    double EvaluateTransportCost(const MPoint& p) const;
    double EvaluateTimeCost(const MPoint& p) const;
    double EvaluateMultiObj(const MPoint& p) const;

    // 约束检查：候选点是否满足所有节点的服务半径要求
    bool CheckServiceRadius(const MPoint& p, double globalMaxKm = 0.0) const;

    // 获取求解参数
    const vector<LogisticsNode>& GetNodes()     const { return m_nodes; }
    const LogisticsCostModel&    GetCostModel() const { return m_costModel; }
    const OptimizationWeights&   GetWeights()   const { return m_weights; }

private:
    // 计算加权重心作为初始点
    MPoint ComputeWeightedCentroid() const;

    // 通用梯度下降（接受任意目标函数，返回局部最优点）
    MPoint GradientDescent(
        const function<double(const MPoint&)>& objective,
        MPoint initialPoint,
        double initialStep = 0.0001,
        int    maxIter     = 2000000,
        double tolerance   = 1e-10) const;

    vector<LogisticsNode> m_nodes;
    LogisticsCostModel    m_costModel;
    OptimizationWeights   m_weights;
};
