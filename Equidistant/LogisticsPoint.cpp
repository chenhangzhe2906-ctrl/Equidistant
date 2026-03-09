#include "pch.h"
#include "LogisticsPoint.h"

LogisticsNode::LogisticsNode()
    : name("未命名节点"),
      longitude(0.0),
      latitude(0.0),
      annualDemandTons(0.0),
      deliveryFreqPerMonth(1.0),
      costRatePerTonKm(1.0),
      maxServiceRadiusKm(2000.0)
{
}

LogisticsNode::LogisticsNode(const string& n,
                             double lon, double lat,
                             double demandTons,
                             double freqPerMonth,
                             double ratePerTonKm,
                             double maxRadiusKm)
    : name(n),
      longitude(lon),
      latitude(lat),
      annualDemandTons(demandTons),
      deliveryFreqPerMonth(freqPerMonth),
      costRatePerTonKm(ratePerTonKm),
      maxServiceRadiusKm(maxRadiusKm)
{
}

MPoint LogisticsNode::ToMPoint() const
{
    // 将年需求量归一化为 0~1 的需求权重（以10000吨为基准）
    double demandWeight = min(1.0, annualDemandTons / 10000.0);
    if (demandWeight <= 0.0) demandWeight = 0.01;
    return MPoint(longitude, latitude, demandWeight, costRatePerTonKm, deliveryFreqPerMonth * 0.01);
}

double LogisticsNode::AnnualTransportCost(double distKm) const
{
    // 年运输成本（元）= 年需求量 × 距离 × 成本系数
    return annualDemandTons * distKm * costRatePerTonKm;
}
