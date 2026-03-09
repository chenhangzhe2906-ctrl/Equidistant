// Console.cpp : 智能物流仓库选址系统演示程序
// 功能：对比等距选址、加权选址、多目标选址三种方案

#include <iostream>
#include <iomanip>
#include <string>
#include "MPoint.h"
#include "Coordinate.h"
#include "LogisticsPoint.h"
#include "LogisticsCostModel.h"
#include "LogisticsSolver.h"
#include "LogisticsData.h"
#include "LogisticsReport.h"

// 打印横线分隔符
static void Sep(char c = '=', int w = 60) {
    for (int i = 0; i < w; i++) std::cout << c;
    std::cout << "\n";
}

int main()
{
    std::cout << "\n";
    Sep('*');
    std::cout << "  Smart Logistics Warehouse Site Selection System  v1.0\n";
    std::cout << "  Powered by Equidistant Core Algorithm (WGS84)\n";
    Sep('*');
    std::cout << "\n";

    // ──────────────────────────────────────────────────────────
    // 1. Load sample data
    // ──────────────────────────────────────────────────────────
    std::cout << "[Step 1] Load Logistics Node Data\n";
    Sep('-');
    auto nodes = LogisticsData::GetSampleChineseCities();
    std::cout << "  Loaded " << nodes.size() << " logistics hub cities:\n";
    std::cout << std::fixed << std::setprecision(1);
    for (const auto& n : nodes) {
        std::cout << "    " << std::setw(10) << std::left << n.name
                  << "  Lon:" << std::setw(7) << std::right << n.longitude
                  << "  Lat:" << std::setw(6) << n.latitude
                  << "  Demand:" << std::setw(7) << n.annualDemandTons << " t/yr"
                  << "  Rate:" << n.costRatePerTonKm << " yuan/t.km\n";
    }
    std::cout << "\n";

    // ──────────────────────────────────────────────────────────
    // 2. Define cost model
    // ──────────────────────────────────────────────────────────
    std::cout << "[Step 2] Configure Logistics Cost Model\n";
    Sep('-');
    // 5000 wan construction, 300 wan/yr operating, 1.2 yuan/t.km, 60 km/h, 50 yuan/h time value
    LogisticsCostModel costModel(5000.0, 300.0, 1.2, 60.0, 50.0);
    std::cout << "  Warehouse construction cost : " << costModel.warehouseConstructionCost << " wan yuan\n";
    std::cout << "  Annual operating cost       : " << costModel.annualOperatingCostBase   << " wan yuan\n";
    std::cout << "  Transport cost rate         : " << costModel.costPerTonKmDefault        << " yuan/t.km\n";
    std::cout << "  Average transport speed     : " << costModel.avgSpeedKmPerHour          << " km/h\n";
    std::cout << "  Time value coefficient      : " << costModel.timeValuePerHour           << " yuan/h\n\n";

    // ──────────────────────────────────────────────────────────
    // 3. Scheme A: Classic equidistant (simpleSolver)
    // ──────────────────────────────────────────────────────────
    std::cout << "[Step 3] Scheme A: Classic Equidistant (minimize distance variance)\n";
    Sep('-');
    MPoint equidistantResult;
    {
        std::vector<MPoint> cityPoints;
        for (const auto& n : nodes) cityPoints.push_back(n.ToMPoint());
        Coordinate coord(cityPoints);
        equidistantResult = coord.simpleSolver();
        std::cout << "  Optimal point: Lon " << equidistantResult.GetLongitude() * RAD_TO_DEG
                  << " E,  Lat " << equidistantResult.GetLatitude() * RAD_TO_DEG << " N\n";
        std::cout << "  Distance variance: " << std::setprecision(2)
                  << coord.CalculateVariance(equidistantResult) << " km^2\n\n";
    }

    // ──────────────────────────────────────────────────────────
    // 4. Scheme B: Weighted equidistant (advancedSolver)
    // ──────────────────────────────────────────────────────────
    std::cout << "[Step 4] Scheme B: Weighted Equidistant (demand-weighted + multi-objective score)\n";
    Sep('-');
    MPoint weightedResult;
    {
        std::vector<MPoint> cityPoints;
        for (const auto& n : nodes) cityPoints.push_back(n.ToMPoint());
        Coordinate coord(cityPoints);
        weightedResult = coord.advancedSolver();
        std::cout << "  Optimal point: Lon " << weightedResult.GetLongitude() * RAD_TO_DEG
                  << " E,  Lat " << weightedResult.GetLatitude() * RAD_TO_DEG << " N\n";
        std::cout << "  Multi-objective score: " << std::setprecision(4)
                  << coord.CalculateScore(weightedResult) << "\n\n";
    }

    // ──────────────────────────────────────────────────────────
    // 5. Scheme C: Full multi-objective logistics solver
    // ──────────────────────────────────────────────────────────
    std::cout << "[Step 5] Scheme C: Multi-Objective Logistics Solver (cost + distance + time)\n";
    Sep('-');

    OptimizationWeights weights(0.4, 0.4, 0.2);
    LogisticsSolver solver(nodes, costModel, weights);

    std::cout << "  Optimization weights: alpha=" << weights.distanceWeight
              << " beta=" << weights.costWeight
              << " gamma=" << weights.timeWeight << "\n";
    std::cout << "  Solving (please wait)...\n";

    MPoint multiObjResult = solver.SolveMultiObj();
    std::cout << "  Optimal point: Lon " << multiObjResult.GetLongitude() * RAD_TO_DEG
              << " E,  Lat " << multiObjResult.GetLatitude() * RAD_TO_DEG << " N\n";
    std::cout << "  Multi-obj score      : " << std::setprecision(6) << solver.EvaluateMultiObj(multiObjResult)        << "\n";
    std::cout << "  Weighted variance    : " << std::setprecision(2) << solver.EvaluateWeightedVariance(multiObjResult) << " km^2\n";
    std::cout << "  Annual transport cost: " << std::setprecision(0) << solver.EvaluateTransportCost(multiObjResult)   << " wan yuan\n";
    std::cout << "  Annual time cost     : " << solver.EvaluateTimeCost(multiObjResult) << " wan yuan\n\n";

    // ──────────────────────────────────────────────────────────
    // 6. Scheme D: Constrained solver (service radius <= 1800 km)
    // ──────────────────────────────────────────────────────────
    std::cout << "[Step 6] Scheme D: Constrained Solver (max service radius 1800 km)\n";
    Sep('-');
    MPoint constrainedResult = solver.SolveConstrained(1800.0);
    bool feasible = solver.CheckServiceRadius(constrainedResult, 1800.0);
    std::cout << "  Optimal point: Lon " << constrainedResult.GetLongitude() * RAD_TO_DEG
              << " E,  Lat " << constrainedResult.GetLatitude() * RAD_TO_DEG << " N\n";
    std::cout << "  Constraint satisfied: " << (feasible ? "YES" : "NO (some nodes exceed radius)") << "\n\n";

    // ──────────────────────────────────────────────────────────
    // 7. Detailed analysis of multi-objective scheme
    // ──────────────────────────────────────────────────────────
    std::cout << "[Step 7] Detailed Analysis - Multi-Objective Scheme\n\n";
    auto analysisEq = LogisticsReport::Analyze(equidistantResult, nodes, costModel);
    auto analysisWt = LogisticsReport::Analyze(weightedResult,    nodes, costModel);
    auto analysisMo = LogisticsReport::Analyze(multiObjResult,    nodes, costModel);
    LogisticsReport::PrintReport(analysisMo, "Multi-Objective Scheme - Detailed Report");

    // ──────────────────────────────────────────────────────────
    // 8. Three-scheme comparison
    // ──────────────────────────────────────────────────────────
    std::cout << "[Step 8] Three-Scheme Comparison\n\n";
    LogisticsReport::PrintComparisonReport(analysisEq, analysisWt, analysisMo);

    // ──────────────────────────────────────────────────────────
    // 9. ROI analysis (multi-objective scheme selected)
    // ──────────────────────────────────────────────────────────
    std::cout << "[Step 9] Return on Investment Analysis (Multi-Objective Scheme)\n\n";
    double baselineCostWan = 8000.0; // baseline annual logistics cost: 8000 wan yuan
    LogisticsReport::PrintROIAnalysis(analysisMo, costModel, 5000.0, baselineCostWan);

    // ──────────────────────────────────────────────────────────
    // 10. CSV export demo
    // ──────────────────────────────────────────────────────────
    std::cout << "[Step 10] CSV Data Export Demo\n";
    Sep('-');
    std::string csvPath = "logistics_nodes.csv";
    if (LogisticsData::SaveToCSV(csvPath, nodes)) {
        std::cout << "  Node data exported to: " << csvPath << "\n";
        auto reloaded = LogisticsData::LoadFromCSV(csvPath);
        std::cout << "  Reloaded node count  : " << reloaded.size() << " (validation OK)\n";
    }
    std::cout << "\n";

    Sep('*');
    std::cout << "  Demo complete.\n";
    std::cout << "  Recommendation: Adopt Multi-Objective scheme for optimal logistics warehouse siting.\n";
    Sep('*');
    std::cout << "\n";

    return 0;
}
