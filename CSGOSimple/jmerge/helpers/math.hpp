#pragma once

#include "../valve_sdk/sdk.hpp"

#include <DirectXMath.h>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)
#define M_PI 3.14159265358979323846
#define PI_F	((float)(M_PI))

#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
#define TICKS_TO_TIME(t) (g_GlobalVars->interval_per_tick * (t) )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

namespace Math
{
	void FixAngles( QAngle &angle );

	inline float FASTSQRT( float x ) {
		unsigned int i = *( unsigned int* )&x;

		i += 127 << 23;
		// approximation of square root
		i >>= 1;
		return *( float* )&i;
	}

	float VectorDistance( const Vector& v1, const Vector& v2 );
	QAngle CalcAngle( const Vector& src, const Vector& dst );
	float GetFOV( const QAngle& viewAngle, const QAngle& aimAngle );

	void NormalizeAngles(QAngle& angles);
	template<class T>
	void Normalize3( T& vec ) {
		for( auto i = 0; i < 2; i++ ) {
			while( vec[i] < -180.0f ) vec[i] += 360.0f;
			while( vec[i] > 180.0f ) vec[i] -= 360.0f;
		}
		vec[2] = 0.f;
	}

	bool IntersectionBoundingBox( const Vector& start, const Vector& dir, const Vector& min, const Vector& max, Vector* hit_point = nullptr );

	void MovementFix(CUserCmd* cmd, QAngle wish_angle, QAngle old_angles );
	float NormalizeYaw(float value);
	void ClampAngles( QAngle& angles );
	void VectorTransform( const Vector& in1, const matrix3x4_t& in2, Vector& out );
	void AngleVectors( const QAngle &angles, Vector& forward );
	void AngleVectors( const QAngle &angles, Vector& forward, Vector& right, Vector& up );
	bool SanitizeAngles( QAngle& angles );
	void VectorAngles2( const Vector& forward, QAngle& angles);
	void AngleVectors2(const QAngle& angles, Vector& forward);
	void VectorAngles( const Vector& forward, QAngle& angles );
	bool WorldToScreen( const Vector& in, Vector& out );
	void RotateTriangle(std::array<Vector2D, 3>& points, float rotation);
	float ClampYaw(float yaw);
}
