#pragma once
#include "pch.h"
#include "MPoint.h"
#include "LogisticsPoint.h"

// 物流成本模型：封装仓库选址中的各类成本参数
class LogisticsCostModel {
public:
    double warehouseConstructionCost; // 仓库建设总投资（万元）
    double annualOperatingCostBase;   // 年基础运营成本（万元/年）
    double costPerTonKmDefault;       // 默认综合运输成本（元/吨·公里）
    double avgSpeedKmPerHour;         // 平均运输速度（公里/小时）
    double timeValuePerHour;          // 时间价值系数（元/小时）

    LogisticsCostModel();
    LogisticsCostModel(double constructionWan,
                       double operatingWan,
                       double perTonKm,
                       double speedKph  = 60.0,
                       double timeValue = 50.0);

    // 单次运输成本（元）= 货物量（吨） × 距离（公里） × 成本系数（元/吨·公里）
    double CalcTransportCost(double distKm, double tons, double rateOverride = -1.0) const;

    // 单次运输时间（小时）
    double CalcTransitHours(double distKm) const;

    // 时间成本（元）= 运输时间 × 频次 × 时间价值系数
    double CalcTimeCost(double distKm, double annualFreq) const;

    // 某节点的年度总物流成本（运输 + 时间，元）
    double CalcNodeAnnualCost(const LogisticsNode& node, double distKm) const;

    // 所有节点的年度总运输成本（元）
    double CalcTotalAnnualTransportCost(
        const vector<LogisticsNode>& nodes, const MPoint& warehouseLoc) const;

    // 投资回报率（%）
    // ROI = (年节约成本 - 年运营成本) / 总投资 × 100
    double CalcROI(double annualCostSaved, double investmentWan) const;

    // 静态回收期（年）= 总投资 / 年净收益
    double CalcPaybackYears(double annualNetBenefit) const;
};
