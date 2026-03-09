#pragma once
#include "pch.h"
#include "MPoint.h"

// 物流节点：代表一个需要配送服务的客户点（城市/仓库/工厂）
struct LogisticsNode {
    string   name;                 // 节点名称
    double   longitude;            // 经度（度）
    double   latitude;             // 纬度（度）
    double   annualDemandTons;     // 年货运需求量（吨）
    double   deliveryFreqPerMonth; // 月均配送频次（次/月）
    double   costRatePerTonKm;     // 运输成本系数（元/吨·公里）
    double   maxServiceRadiusKm;   // 最大可接受服务半径（公里）

    LogisticsNode();
    LogisticsNode(const string& n,
                  double lon, double lat,
                  double demandTons,
                  double freqPerMonth,
                  double ratePerTonKm,
                  double maxRadiusKm = 2000.0);

    // 转换为 MPoint（将业务属性映射到物理坐标点）
    MPoint ToMPoint() const;

    // 计算该节点到候选仓库的年运输成本（元）
    double AnnualTransportCost(double distKm) const;
};

// 物流选址结果：仓库候选点的完整评估信息
struct WarehouseCandidate {
    MPoint   location;
    double   longitude;
    double   latitude;
    double   totalAnnualTransportCost;  // 年总运输成本（万元）
    double   totalAnnualFixedCost;      // 年固定运营成本（万元）
    double   totalAnnualCost;           // 年总成本（万元）
    double   averageDistanceKm;         // 各节点平均距离（公里）
    double   maxDistanceKm;             // 最远节点距离（公里）
    double   minDistanceKm;             // 最近节点距离（公里）
    double   weightedAvgDistanceKm;     // 按需求量加权的平均距离（公里）
    double   distanceVariance;          // 等距方差（平方公里）
    double   roiPercent;                // 投资回报率（%）
    vector<pair<string, double>> nodeDistances;    // 各节点距离明细
    vector<pair<string, double>> nodeAnnualCosts;  // 各节点年运输成本明细（万元）
};
