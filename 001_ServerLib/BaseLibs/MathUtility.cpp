
#include "MathUtility.hpp"

//Quake3ʹ�õĿ���ƽ�����㷨
float MathUtility::FastSqrtQ3(float x)
{
#define SQRT_MAGIC_F 0x5f3759df

	const float xhalf = 0.5f*x;
	union
	{
		float x;
		int i;
	} u;
	u.x = x;
	u.i = SQRT_MAGIC_F - (u.i >> 1);
	return x * u.x * (1.5f - xhalf * u.x * u.x);
}