#pragma once

#include "pch.h"

class MVector
{
public:
	MVector(void);
	MVector(double x, double y, double z);
	~MVector(void);

public:
	MVector& operator =(const MVector& vec);  // 将 TcVector 改为 MVector
	double operator [](int idx) const;
	//double& operator [](int idx);
	void Set(double x, double y, double z);
	double operator %(const MVector& vec) const;
	MVector operator *(const MVector& vec) const;
	MVector& operator *= (double d) { m_coord[0] *= d; m_coord[1] *= d; m_coord[2] *= d; return *this; }
	MVector& operator /= (double d) { return operator *= (1.0 / d); }
	MVector& operator += (const MVector& v) { m_coord[0] += v[0]; m_coord[1] += v[1]; m_coord[2] += v[2]; return *this; }
	MVector& operator -= (const MVector& v) { m_coord[0] -= v[0]; m_coord[1] -= v[1]; m_coord[2] -= v[2]; return *this; }
	MVector operator -() const { return MVector(-m_coord[0], -m_coord[1], -m_coord[2]); }

	friend bool operator==(const MVector& vec1, const MVector& vec2)
	{
		return	  ((fabs(vec1[0] - vec2[0]) < 1.0e-7f)
			&& (fabs(vec1[1] - vec2[1]) < 1.0e-7f)
			&& (fabs(vec1[2] - vec2[2]) < 1.0e-7f));
	}
	friend bool operator != (const MVector& vec1, const MVector& vec2)
	{
		return !(vec1 == vec2);
	}

public:
	double Magnitude() const;
	double Length2() const;
	void Normalize();
	bool IsZero() const;
	void SetZero() { Set(0.0, 0.0, 0.0); };
	void Flip() { Set(-m_coord[0], -m_coord[1], -m_coord[2]); };

	/*
	double AngleWith(const MVector& vec) const;
	double AngleWith(const MVector& vec, const MVector& nor) const;
	void GetValue(double* d) const;
	bool IsParallelTo(const MVector& vec) const;
	*/

private:
	double m_coord[3];

public:
	friend MVector operator *(double r, const MVector& vec)
	{
		return MVector(r * vec[0], r * vec[1], r * vec[2]);
	}

	friend MVector operator *(const MVector& vec, double r)
	{
		return MVector(r * vec[0], r * vec[1], r * vec[2]);
	}

	friend MVector operator /(const MVector& vec, double r)
	{
		return MVector(vec[0] / r, vec[1] / r, vec[2] / r);
	}

	friend MVector operator +(const MVector& vec1, const MVector& vec2)
	{
		return MVector(vec1[0] + vec2[0], vec1[1] + vec2[1], vec1[2] + vec2[2]);
	}

	friend MVector operator -(const MVector& vec1, const MVector& vec2)
	{
		return MVector(vec1[0] - vec2[0], vec1[1] - vec2[1], vec1[2] - vec2[2]);
	}

	// 注意：这里使用了 TcMatrix，可能需要保留或者修改
	// 如果不需要这个函数，可以注释掉
	/*
	friend MVector operator *(const TcMatrix& mat, const MVector& vec)
	{
		//ASSERT(mat.GetRow() == 4 && mat.GetCol() == 4);

		return MVector(mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2],
			mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2],
			mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2]);
	}
	*/
};


inline double MVector::operator %(const MVector& vec) const
{
	return m_coord[0] * vec[0] + m_coord[1] * vec[1] + m_coord[2] * vec[2];
}


inline MVector MVector::operator *(const MVector& vec) const
{
	return MVector(m_coord[1] * vec[2] - m_coord[2] * vec[1],
		m_coord[2] * vec[0] - m_coord[0] * vec[2],
		m_coord[0] * vec[1] - m_coord[1] * vec[0]);
}