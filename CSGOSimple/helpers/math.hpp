#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../valve_sdk/sdk.hpp"

#include <DirectXMath.h>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)
#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( g_GlobalVars->interval_per_tick *( t ) )
#define M_PI 3.14159265358979323846
#define PI_F	((float)(M_PI)) 

namespace Math
{
	inline float FASTSQRT(float x)
	{
		unsigned int i = *(unsigned int*)&x;

		i += 127 << 23;
		// approximation of square root
		i >>= 1;
		return *(float*)&i;
	}
	float RandomFloat(float min, float max);
	void VectorAngles1337(const Vector& forward, Vector& up, QAngle& angles);
	float VectorDistance(const Vector& v1, const Vector& v2);
	float DistancePointToLine(Vector Point, Vector LineOrigin, Vector Dir);
	QAngle CalcAngle(const Vector& src, const Vector& dst);
	float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle);
	template<class T>
	void Normalize3(T& vec)
	{
		for (auto i = 0; i < 2; i++) {
			while (vec[i] < -180.0f) vec[i] += 360.0f;
			while (vec[i] >  180.0f) vec[i] -= 360.0f;
		}
		vec[2] = 0.f;
	}
	template<class T, class U>
	static T clamp(const T& in, const U& low, const U& high)
	{
		if (in <= low)
			return low;

		if (in >= high)
			return high;

		return in;
	}
	int random_int(int min, int max);
    void ClampAngles(QAngle& angles);
	void FixAngles(QAngle& angles);
    void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	float normalize_yaw(float f);
	void angle_vectors(const Vector& angles, Vector& forward);
	void angle_vectors(const Vector& angles, Vector* forward, Vector* right, Vector* up);
	void vector_angles(const Vector& forward, Vector& angles);
	void vector_angles(const Vector& forward, Vector& up, Vector& angles);
    void AngleVectors(const QAngle &angles, Vector& forward);
    void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up);
    void VectorAngles(const Vector& forward, QAngle& angles);
    bool WorldToScreen(const Vector& in, Vector& out);
	void CorrectMovement(CUserCmd* cmd, QAngle wish_angle, QAngle old_angles);
	float Magnitude(Vector a);
	Vector Normalize(Vector value);
	Vector ClampMagnitude(Vector vector, float maxLength);
	float random_float(float min, float max);

	template <typename t>
	static t interpolate(const t& t1, const t& t2, float progress)
	{
		if (t1 == t2)
			return t1;

		return t2 * progress + t1 * (1.0f - progress);
	}

}