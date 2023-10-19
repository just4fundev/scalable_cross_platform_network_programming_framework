// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <cmath>

#include "FloatComparison.h"

namespace GameServer
{
	inline class Vector3 operator * (const float& value, const class Vector3& v);

	class Vector3
	{
	public:
		explicit Vector3(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		Vector3 operator + (const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
		Vector3 operator - (const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
		Vector3 operator * (const float& value) const { return Vector3(x * value, y - value, z - value); }
		Vector3 operator / (const float& value) const { return Vector3(x / value, y / value, z / value); }

		void operator = (const Vector3& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}

		void operator += (const Vector3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
		}

		void operator -= (const Vector3& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
		}

		void operator *= (const float& v)
		{
			x *= v;
			y *= v;
			z *= v;
		}

		void operator /= (const float& v)
		{
			x /= v;
			y /= v;
			z /= v;
		}

		bool operator == (const Vector3& v)
		{
			return FloatComparison::Equal(x, v.x) && FloatComparison::Equal(y, v.y) && FloatComparison::Equal(z, v.z);
		}

		bool operator != (const Vector3& v)
		{
			return !((*this) == v);
		}

		static float Dot(const Vector3& v1, const Vector3& v2)
		{
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		}

		static Vector3 Cross(const Vector3& v1, const Vector3& v2)
		{
			const float x = (v1.y * v2.z) - (v1.z * v2.y);
			const float y = (v1.z * v2.x) - (v1.x * v2.z);
			const float z = (v1.x * v2.y) - (v1.y * v2.x);

			return Vector3(x, y, z);
		}

		static float Angle(const Vector3& v1, const Vector3& v2)
		{
			const float dot = Dot(v1, v2);
			const float length = Distance(v1, v2);

			return std::acos(dot / length);
		}

		static float Length(const Vector3& v)
		{
			return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		}

		static float Distance(const Vector3& v1, const Vector3& v2)
		{
			const Vector3 v = v1 - v2;
			return Length(v);
		}

		static Vector3 Normalized(const Vector3& v)
		{
			const float length = Length(v);

			return Vector3(v.x / length, v.y / length, v.z / length);
		}

		static Vector3 Project(const Vector3& from, const Vector3& to)
		{
			const float dot = Dot(from, to);
			const float length = Length(to);

			return (dot / (length * length)) * to;
		}

		static const Vector3 Up;
		static const Vector3 Down;
		static const Vector3 Forward;
		static const Vector3 Back;
		static const Vector3 Right;
		static const Vector3 Left;

		float x;
		float y;
		float z;
	};

	Vector3 operator * (const float& value, const Vector3& v) { return v * value; }
}