#include "pch.h"
#include "MPoint.h"

MPoint::MPoint()
{
	Set(0.0, 0.0, 0.0);
	L = 0.0;
	B = 0.0;
	N = 0.0;
	demandWeight = 1.0;
	costMoney = 1.0;
	costTime = 1.0;
}

MPoint::~MPoint()
{

}

MPoint::MPoint(double L, double B)
{
	this->L = L;
	this->B = B;
	Get_XYZ(L, B);
}

MPoint::MPoint(double L, double B,  double dW, double cM, double cT)
{
	this->L = L;
	this->B = B;
	Get_XYZ(L, B);
	extraInit(dW, cM, cT);
}

MPoint::MPoint(double x, double y, double z)
{
	Set(x, y, z);
	this->L = Get_LB(x, y, z)[0];
	this->B = Get_LB(x, y, z)[1];
	return;
}

MPoint::MPoint(double x, double y, double z,double dW,double cM,double cT)
{
	Set(x, y, z);
	this->L = Get_LB(x, y, z)[0];
	this->B = Get_LB(x, y, z)[1];
	extraInit(dW, cM, cT);
	return;
}

void MPoint::Set(double x, double y, double z)
{
	this->X = x;
	this->Y = y;
	this->Z = z;
	return;
}

void MPoint::extraInit(double dW,double cM, double cT)
{
	this->demandWeight = dW;
	this->costMoney = cM;
	this->costTime = cT;
	return;
}

vector<double> MPoint::Get_XYZ() const
{
	return vector <double> {X, Y, Z};
}

vector<double> MPoint::Get_XYZ(double L, double B)
{
	this->N = MAJORAXIS / sqrt(1 - ECCENTRICITY * ECCENTRICITY * sin(B * DEG_TO_RAD) * sin(B * DEG_TO_RAD));
	this->X = N * cos(B* DEG_TO_RAD) * cos(L * DEG_TO_RAD);
	this->Y = N * cos(B * DEG_TO_RAD) * sin(L * DEG_TO_RAD);
	this->Z = N * (1 - ECCENTRICITY * ECCENTRICITY) * sin(B * DEG_TO_RAD);
	return { X,Y,Z };
}

vector<double> MPoint::Get_LB() const
{
	return vector<double>{L, B};
}

vector<double> MPoint::Get_LB(double x, double y, double z)
{
	L = atan2(y,x);
	if (x == 0 && y == 0) {
		B = (z >= 0) ? PI / 2.0 : -PI / 2.0;
		N = MAJORAXIS / sqrt(1 - ECCENTRICITY * ECCENTRICITY * sin(B) * sin(B));
		return vector <double> {L, B};
	}
	const double epsilon = tan(0.0000000001); //��΢�׼����
	double temp=1.0;
	int count = 0;
	double t_n, r, l;
	double t_b = atan(z / sqrt(x * x + y * y));
	while (fabs(temp) > epsilon && count<10000) {
		t_n = MAJORAXIS / sqrt(1 - ECCENTRICITY * ECCENTRICITY * sin(t_b) * sin(t_b));
		r = (z + t_n * ECCENTRICITY * ECCENTRICITY * sin(t_b)) / sqrt(x * x + y * y);
		l = tan(t_b);
		temp = r - l;
		t_b = atan(r);
		count += 1;
	}
	B = t_b;
	N = t_n;
	return vector <double> {L, B};
}

void MPoint::Move(double m_L, double m_B)
{
	L += m_L;
	B += m_B;
	return;
}

double MPoint::DistanceTo(const MPoint& other) const
{
	double lat1_rad = this->B * DEG_TO_RAD;
	double lon1_rad = this->L * DEG_TO_RAD;
	double lat2_rad = other.B * DEG_TO_RAD;
	double lon2_rad = other.L * DEG_TO_RAD;

	double delta_lat = lat2_rad - lat1_rad;
	double delta_lon = lon2_rad - lon1_rad;

	double a = sin(delta_lat / 2.0) * sin(delta_lat / 2.0) +
		cos(lat1_rad) * cos(lat2_rad) *
		sin(delta_lon / 2.0) * sin(delta_lon / 2.0);

	double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

	return R * c; // ���ؾ��룬��λ������
}

MPoint & MPoint::operator=(const MPoint& point)
{
	if (this != &point) {
		L = point.L;
		B = point.B;
		X = point.X;
		Y = point.Y;
		Z = point.Z; 
		N = point.N;
		demandWeight = point.demandWeight;
		costMoney    = point.costMoney;
		costTime     = point.costTime;
	}
	return *this;
}
