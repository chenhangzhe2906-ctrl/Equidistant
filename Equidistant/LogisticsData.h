#pragma once
#include "pch.h"
#include "LogisticsPoint.h"

// 物流数据 I/O：支持 CSV 格式的读写以及内置样本数据集
class LogisticsData {
public:
    // 从 CSV 文件加载节点列表
    // CSV 列格式：name,longitude,latitude,demand_tons,freq_per_month,cost_rate,max_radius_km
    static vector<LogisticsNode> LoadFromCSV(const string& filename);

    // 将节点列表保存为 CSV 文件
    static bool SaveToCSV(const string& filename, const vector<LogisticsNode>& nodes);

    // 内置样本数据：中国8大物流枢纽城市
    static vector<LogisticsNode> GetSampleChineseCities();

    // 内置样本数据：华东区域配送网络
    static vector<LogisticsNode> GetSampleEastChinaNetwork();
};
