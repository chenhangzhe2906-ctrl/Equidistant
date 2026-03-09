#include "pch.h"
#include "LogisticsReport.h"

// ── 私有工具函数 ─────────────────────────────────────────────

void LogisticsReport::PrintSeparator(char ch, int width)
{
    cout << string(width, ch) << "\n";
}

string LogisticsReport::FormatDistance(double km)
{
    ostringstream oss;
    oss << fixed << setprecision(1) << km << " km";
    return oss.str();
}

string LogisticsReport::FormatCost(double yuan)
{
    ostringstream oss;
    if (yuan >= 1e8) {
        oss << fixed << setprecision(2) << yuan / 1e8 << " 亿元";
    } else if (yuan >= 1e4) {
        oss << fixed << setprecision(2) << yuan / 1e4 << " 万元";
    } else {
        oss << fixed << setprecision(0) << yuan << " 元";
    }
    return oss.str();
}

// ── 核心分析函数 ─────────────────────────────────────────────

WarehouseCandidate LogisticsReport::Analyze(
    const MPoint&              warehouseLoc,
    const vector<LogisticsNode>& nodes,
    const LogisticsCostModel&  costModel,
    double                     annualRevenueSaved)
{
    WarehouseCandidate result;
    result.location  = warehouseLoc;
    // Warehouse points from ToEarthSurface() store L/B in radians; convert to degrees
    result.longitude = warehouseLoc.GetLongitude() * RAD_TO_DEG;
    result.latitude  = warehouseLoc.GetLatitude()  * RAD_TO_DEG;

    double totalAnnualTransport = 0.0;
    double totalWeight          = 0.0;
    double sumDist              = 0.0;
    double maxDist              = 0.0;
    double minDist              = numeric_limits<double>::max();
    double weightedSumDist      = 0.0;
    double avgDist              = 0.0;

    // 第一遍：计算各节点距离和成本
    vector<pair<string, double>> nodeDistances;
    vector<pair<string, double>> nodeAnnualCosts;

    for (const auto& node : nodes) {
        double dist = CalcHaversineKm(node.longitude, node.latitude,
                                      result.longitude, result.latitude);

        double annualCost = costModel.CalcNodeAnnualCost(node, dist); // 元

        nodeDistances.emplace_back(node.name, dist);
        nodeAnnualCosts.emplace_back(node.name, annualCost / 10000.0); // 转万元

        totalAnnualTransport += annualCost;
        totalWeight          += node.annualDemandTons;
        weightedSumDist      += node.annualDemandTons * dist;
        sumDist              += dist;
        if (dist > maxDist) maxDist = dist;
        if (dist < minDist) minDist = dist;
    }

    // 第二遍：计算等距方差
    avgDist = (nodes.empty()) ? 0.0 : sumDist / nodes.size();
    double variance = 0.0;
    for (const auto& pr : nodeDistances) {
        double diff = pr.second - avgDist;
        variance += diff * diff;
    }
    if (!nodes.empty()) variance /= nodes.size();

    result.nodeDistances          = nodeDistances;
    result.nodeAnnualCosts        = nodeAnnualCosts;
    result.totalAnnualTransportCost = totalAnnualTransport / 10000.0; // 万元
    result.totalAnnualFixedCost   = costModel.annualOperatingCostBase;
    result.totalAnnualCost        = result.totalAnnualTransportCost + result.totalAnnualFixedCost;
    result.averageDistanceKm      = avgDist;
    result.maxDistanceKm          = (minDist == numeric_limits<double>::max()) ? 0.0 : maxDist;
    result.minDistanceKm          = (minDist == numeric_limits<double>::max()) ? 0.0 : minDist;
    result.weightedAvgDistanceKm  = (totalWeight > 0.0) ? weightedSumDist / totalWeight : 0.0;
    result.distanceVariance       = variance;

    // ROI 计算
    if (annualRevenueSaved > 0.0) {
        double annualNet = annualRevenueSaved - result.totalAnnualCost;
        result.roiPercent = costModel.CalcROI(annualNet + result.totalAnnualFixedCost,
                                              costModel.warehouseConstructionCost);
    } else {
        result.roiPercent = 0.0;
    }

    return result;
}

// ── 报告打印函数 ─────────────────────────────────────────────

void LogisticsReport::PrintReport(const WarehouseCandidate& c, const string& title)
{
    PrintSeparator('=', 60);
    cout << "  " << title << "\n";
    PrintSeparator('=', 60);

    cout << fixed << setprecision(4);
    cout << "  仓库坐标：经度 " << c.longitude << "°E，纬度 " << c.latitude << "°N\n";
    PrintSeparator('-', 60);

    cout << "  【距离分析】\n";
    cout << "    平均距离（等权）   : " << FormatDistance(c.averageDistanceKm)     << "\n";
    cout << "    加权平均距离       : " << FormatDistance(c.weightedAvgDistanceKm) << "\n";
    cout << "    最远节点距离       : " << FormatDistance(c.maxDistanceKm)         << "\n";
    cout << "    最近节点距离       : " << FormatDistance(c.minDistanceKm)         << "\n";
    cout << setprecision(2);
    cout << "    等距方差           : " << c.distanceVariance << " km²\n";
    PrintSeparator('-', 60);

    cout << "  【各节点距离明细】\n";
    for (const auto& pr : c.nodeDistances) {
        cout << "    " << setw(8) << left << pr.first << " : "
             << FormatDistance(pr.second) << "\n";
    }
    PrintSeparator('-', 60);

    cout << "  【成本分析】\n";
    cout << "    年运输成本总计     : " << FormatCost(c.totalAnnualTransportCost * 10000.0) << "\n";
    cout << "    年固定运营成本     : " << FormatCost(c.totalAnnualFixedCost     * 10000.0) << "\n";
    cout << "    年总物流成本       : " << FormatCost(c.totalAnnualCost          * 10000.0) << "\n";
    PrintSeparator('-', 60);

    cout << "  【各节点年运输成本】\n";
    for (const auto& pr : c.nodeAnnualCosts) {
        cout << "    " << setw(8) << left << pr.first << " : "
             << FormatCost(pr.second * 10000.0) << "\n";
    }
    if (c.roiPercent > 0.0) {
        PrintSeparator('-', 60);
        cout << "  【投资回报】\n";
        cout << setprecision(1);
        cout << "    投资回报率         : " << c.roiPercent << " %\n";
    }
    PrintSeparator('=', 60);
    cout << "\n";
}

void LogisticsReport::PrintComparisonReport(
    const WarehouseCandidate& equidistant,
    const WarehouseCandidate& weighted,
    const WarehouseCandidate& multiObj)
{
    PrintSeparator('*', 60);
    cout << "  多方案对比分析\n";
    PrintSeparator('*', 60);

    cout << left
         << setw(20) << "指标"
         << setw(14) << "等距方案"
         << setw(14) << "加权方案"
         << setw(14) << "多目标方案"
         << "\n";
    PrintSeparator('-', 60);

    auto row = [&](const string& label, double v1, double v2, double v3, int prec = 1) {
        cout << setprecision(prec)
             << setw(20) << left << label
             << setw(14) << right << v1
             << setw(14) << v2
             << setw(14) << v3
             << "\n";
    };

    cout << fixed;
    row("平均距离 (km)",       equidistant.averageDistanceKm,     weighted.averageDistanceKm,     multiObj.averageDistanceKm);
    row("最远距离 (km)",       equidistant.maxDistanceKm,         weighted.maxDistanceKm,         multiObj.maxDistanceKm);
    row("加权均距 (km)",       equidistant.weightedAvgDistanceKm, weighted.weightedAvgDistanceKm, multiObj.weightedAvgDistanceKm);
    row("等距方差 (km²)",      equidistant.distanceVariance,      weighted.distanceVariance,      multiObj.distanceVariance, 0);
    row("年运输成本 (万元)",   equidistant.totalAnnualTransportCost, weighted.totalAnnualTransportCost, multiObj.totalAnnualTransportCost, 0);
    row("年总成本 (万元)",     equidistant.totalAnnualCost,       weighted.totalAnnualCost,       multiObj.totalAnnualCost, 0);

    PrintSeparator('*', 60);

    // 推荐方案
    double minCost = min({equidistant.totalAnnualCost, weighted.totalAnnualCost, multiObj.totalAnnualCost});
    cout << "\n  推荐方案：";
    if      (minCost == multiObj.totalAnnualCost)   cout << "多目标优化方案（综合成本最优）\n";
    else if (minCost == weighted.totalAnnualCost)    cout << "加权方案（综合成本最优）\n";
    else                                             cout << "等距方案（综合成本最优）\n";
    cout << "\n";
}

void LogisticsReport::PrintROIAnalysis(
    const WarehouseCandidate& selected,
    const LogisticsCostModel& costModel,
    double                    totalInvestmentWan,
    double                    baselineCostWan)
{
    PrintSeparator('=', 60);
    cout << "  投资回报率（ROI）分析\n";
    PrintSeparator('=', 60);

    double annualSaving   = baselineCostWan - selected.totalAnnualCost;
    double annualNet      = annualSaving - costModel.annualOperatingCostBase;
    double roi            = costModel.CalcROI(annualSaving, totalInvestmentWan);
    double paybackYears   = costModel.CalcPaybackYears(annualNet);

    cout << fixed << setprecision(2);
    cout << "  总投资额           : " << totalInvestmentWan          << " 万元\n";
    cout << "  基准年物流成本     : " << baselineCostWan             << " 万元\n";
    cout << "  优化后年总成本     : " << selected.totalAnnualCost    << " 万元\n";
    cout << "  年成本节约         : " << annualSaving                << " 万元\n";
    cout << "  年净收益           : " << annualNet                   << " 万元\n";
    cout << "  投资回报率         : " << roi                         << " %\n";
    if (paybackYears > 0.0) {
        cout << "  静态回收期         : " << paybackYears << " 年\n";
    } else {
        cout << "  静态回收期         : 投资无法回收（成本节约不足）\n";
    }
    PrintSeparator('=', 60);
    cout << "\n";
}
