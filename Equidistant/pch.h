// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

#define PI 3.1415926
#define ECCENTRICITY 0.0818191908426
#define MAJORAXIS 6378.137
#define MINORAXIS 6356.752314245
constexpr double DEG_TO_RAD = PI / 180.0; //角度转弧度
constexpr double RAD_TO_DEG = (180 / PI); //弧度转角度
const double EPSILON = 1e-12;
const double R = 6371.0088;

// 添加要在此处预编译的标头

#include "framework.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <algorithm>
#include <numeric>
#include <map>
#include <limits>
using namespace std;

// 精确 Haversine 大圆距离（公里），输入经纬度均为度数
inline double CalcHaversineKm(double lon1_deg, double lat1_deg,
                               double lon2_deg, double lat2_deg)
{
    constexpr double _R = 6371.0088;
    constexpr double _D2R = PI / 180.0;
    double la1 = lat1_deg * _D2R, lo1 = lon1_deg * _D2R;
    double la2 = lat2_deg * _D2R, lo2 = lon2_deg * _D2R;
    double dLat = la2 - la1, dLon = lo2 - lo1;
    double a = sin(dLat / 2) * sin(dLat / 2)
             + cos(la1) * cos(la2) * sin(dLon / 2) * sin(dLon / 2);
    return _R * 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
}


#endif //PCH_H
