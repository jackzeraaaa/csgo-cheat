#pragma once

class CMatrix3x4
{
public:
	float data[3][4];

	constexpr CVector at(const int index) const
	{
		return CVector(data[0][index], data[1][index], data[2][index]);
	}
};
