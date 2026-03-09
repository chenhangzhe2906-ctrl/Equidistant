#include "pch.h"
#include "LogisticsData.h"

vector<LogisticsNode> LogisticsData::LoadFromCSV(const string& filename)
{
    vector<LogisticsNode> result;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[LogisticsData] 无法打开文件: " << filename << endl;
        return result;
    }

    string line;
    bool firstLine = true;
    while (getline(file, line)) {
        // 跳过空行和标题行
        if (line.empty()) continue;
        if (firstLine) {
            firstLine = false;
            if (line.find("name") != string::npos || line.find("名称") != string::npos)
                continue;
        }

        // 移除 Windows 换行符 \r
        if (!line.empty() && line.back() == '\r') line.pop_back();

        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 6) continue;

        LogisticsNode node;
        node.name                 = tokens[0];
        node.longitude            = stod(tokens[1]);
        node.latitude             = stod(tokens[2]);
        node.annualDemandTons     = stod(tokens[3]);
        node.deliveryFreqPerMonth = stod(tokens[4]);
        node.costRatePerTonKm     = stod(tokens[5]);
        node.maxServiceRadiusKm   = (tokens.size() >= 7) ? stod(tokens[6]) : 2000.0;
        result.push_back(node);
    }
    return result;
}

bool LogisticsData::SaveToCSV(const string& filename, const vector<LogisticsNode>& nodes)
{
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "[LogisticsData] 无法创建文件: " << filename << endl;
        return false;
    }

    // 写入标题行
    file << "name,longitude,latitude,demand_tons,freq_per_month,cost_rate,max_radius_km\n";
    file << fixed << setprecision(4);
    for (const auto& node : nodes) {
        file << node.name             << ","
             << node.longitude        << ","
             << node.latitude         << ","
             << node.annualDemandTons << ","
             << node.deliveryFreqPerMonth << ","
             << node.costRatePerTonKm << ","
             << node.maxServiceRadiusKm  << "\n";
    }
    return true;
}

// 中国8大物流枢纽城市样本数据
// 数据来源：参考公开物流行业报告及城市货运规模估算
vector<LogisticsNode> LogisticsData::GetSampleChineseCities()
{
    // name, 经度, 纬度, 年需求量(吨), 月配送频次, 成本系数(元/吨·公里), 最大服务半径(公里)
    return {
        LogisticsNode("北京",   116.4,  39.9,  8000.0, 40, 1.2, 2000.0),
        LogisticsNode("上海",   121.5,  31.2, 12000.0, 60, 1.0, 2000.0),
        LogisticsNode("广州",   113.3,  23.1,  9000.0, 50, 1.1, 2000.0),
        LogisticsNode("成都",   104.1,  30.6,  5000.0, 25, 1.3, 2000.0),
        LogisticsNode("武汉",   114.3,  30.6,  4500.0, 22, 1.2, 2000.0),
        LogisticsNode("西安",   108.9,  34.3,  3500.0, 18, 1.3, 2000.0),
        LogisticsNode("重庆",   106.5,  29.6,  4000.0, 20, 1.2, 2000.0),
        LogisticsNode("南京",   118.8,  32.1,  3800.0, 19, 1.1, 2000.0),
    };
}

// 华东区域配送网络样本数据（适合区域仓分析）
vector<LogisticsNode> LogisticsData::GetSampleEastChinaNetwork()
{
    return {
        LogisticsNode("上海",   121.5,  31.2,  10000.0, 80, 1.0, 800.0),
        LogisticsNode("南京",   118.8,  32.1,   4000.0, 30, 1.1, 800.0),
        LogisticsNode("杭州",   120.2,  30.3,   5000.0, 35, 1.0, 800.0),
        LogisticsNode("苏州",   120.6,  31.3,   3500.0, 25, 1.0, 800.0),
        LogisticsNode("宁波",   121.6,  29.9,   3000.0, 22, 1.1, 800.0),
        LogisticsNode("无锡",   120.3,  31.6,   2500.0, 18, 1.1, 800.0),
        LogisticsNode("合肥",   117.3,  31.9,   2000.0, 15, 1.2, 800.0),
        LogisticsNode("南通",   120.9,  32.0,   1800.0, 12, 1.2, 800.0),
    };
}
