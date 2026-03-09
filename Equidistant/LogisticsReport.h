#pragma once
#include "pch.h"
#include "MPoint.h"
#include "LogisticsPoint.h"
#include "LogisticsCostModel.h"
#include "LogisticsSolver.h"

// 物流选址报告生成器
class LogisticsReport {
public:
    // 对候选仓库位置进行完整分析，返回分析结构体
    static WarehouseCandidate Analyze(
        const MPoint&              warehouseLoc,
        const vector<LogisticsNode>& nodes,
        const LogisticsCostModel&  costModel,
        double                     annualRevenueSaved = 0.0);

    // 打印单个候选点的分析报告
    static void PrintReport(const WarehouseCandidate& candidate,
                            const string& title = "仓库选址分析报告");

    // 打印多方案对比报告（等距方案 vs 加权方案 vs 多目标方案）
    static void PrintComparisonReport(
        const WarehouseCandidate& equidistant,
        const WarehouseCandidate& weighted,
        const WarehouseCandidate& multiObj);

    // 打印投资回报分析
    static void PrintROIAnalysis(
        const WarehouseCandidate& selected,
        const LogisticsCostModel& costModel,
        double                    totalInvestmentWan,
        double                    baselineCostWan);

private:
    static void PrintSeparator(char ch = '=', int width = 60);
    static string FormatDistance(double km);
    static string FormatCost(double yuan);
};
