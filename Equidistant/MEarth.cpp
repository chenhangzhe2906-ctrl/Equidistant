#include "pch.h"
#include "MEarth.h"

MEarth::MEarth(MVector vec):MEllipse(6378.137, 6356.752)
{
    d_vec = vec;
    angle = atan2(vec[1], vec[0]);
}

MEarth::~MEarth()
{
}

MPoint MEarth::ToEarthSurface()
{
    double t_x, t_y;
    double x, y, z;
    double k;
    if (fabs(d_vec[1]) <= EPSILON && fabs(d_vec[0]) <= EPSILON) {
        double sign = (d_vec[2] > 0) ? 1.0 : -1.0;
        return MPoint(0.0, 0.0, m_b*sign);
    }
    else if(fabs(d_vec[1]) <= EPSILON && fabs(d_vec[2]) <= EPSILON) {
        double sign = (d_vec[0] > 0) ? 1.0 : -1.0;
        return MPoint(m_a * sign, 0.0, 0.0);
    }
    else if (fabs(d_vec[1]) <= EPSILON) {
        k = d_vec[2] * cos(angle) / d_vec[0];
    }
    else {
        k = d_vec[2] * sin(angle) / d_vec[1];
    }
    t_x = sqrt(m_a * m_a * m_b * m_b / (m_a * m_a * k * k + m_b * m_b));
    t_y = k * t_x;
    z = t_y ;
    y = t_x * sin(angle);
    x = t_x * cos(angle);
    return MPoint(x,y,z);
}
