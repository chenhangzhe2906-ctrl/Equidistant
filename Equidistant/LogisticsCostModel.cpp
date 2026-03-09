#include "pch.h"
#include "LogisticsCostModel.h"

LogisticsCostModel::LogisticsCostModel()
    : warehouseConstructionCost(5000.0),
      annualOperatingCostBase(300.0),
      costPerTonKmDefault(1.2),
      avgSpeedKmPerHour(60.0),
      timeValuePerHour(50.0)
{
}

LogisticsCostModel::LogisticsCostModel(double constructionWan,
                                       double operatingWan,
                                       double perTonKm,
                                       double speedKph,
                                       double timeValue)
    : warehouseConstructionCost(constructionWan),
      annualOperatingCostBase(operatingWan),
      costPerTonKmDefault(perTonKm),
      avgSpeedKmPerHour(speedKph),
      timeValuePerHour(timeValue)
{
}

double LogisticsCostModel::CalcTransportCost(double distKm, double tons, double rateOverride) const
{
    double rate = (rateOverride > 0.0) ? rateOverride : costPerTonKmDefault;
    return tons * distKm * rate;
}

double LogisticsCostModel::CalcTransitHours(double distKm) const
{
    if (avgSpeedKmPerHour <= 0.0) return 0.0;
    return distKm / avgSpeedKmPerHour;
}

double LogisticsCostModel::CalcTimeCost(double distKm, double annualFreq) const
{
    return CalcTransitHours(distKm) * annualFreq * timeValuePerHour;
}

double LogisticsCostModel::CalcNodeAnnualCost(const LogisticsNode& node, double distKm) const
{
    double annualFreq = node.deliveryFreqPerMonth * 12.0;
    double transport  = CalcTransportCost(distKm, node.annualDemandTons, node.costRatePerTonKm);
    double time       = CalcTimeCost(distKm, annualFreq);
    return transport + time;
}

double LogisticsCostModel::CalcTotalAnnualTransportCost(
    const vector<LogisticsNode>& nodes, const MPoint& warehouseLoc) const
{
    double wLon = warehouseLoc.GetLongitude() * RAD_TO_DEG;
    double wLat = warehouseLoc.GetLatitude()  * RAD_TO_DEG;
    double total = 0.0;
    for (const auto& node : nodes) {
        double dist = CalcHaversineKm(node.longitude, node.latitude, wLon, wLat);
        total += CalcNodeAnnualCost(node, dist);
    }
    return total;
}

double LogisticsCostModel::CalcROI(double annualCostSaved, double investmentWan) const
{
    if (investmentWan <= 0.0) return 0.0;
    double annualNet = annualCostSaved - annualOperatingCostBase;
    return (annualNet / investmentWan) * 100.0;
}

double LogisticsCostModel::CalcPaybackYears(double annualNetBenefit) const
{
    if (annualNetBenefit <= 0.0) return -1.0;
    return warehouseConstructionCost / annualNetBenefit;
}
