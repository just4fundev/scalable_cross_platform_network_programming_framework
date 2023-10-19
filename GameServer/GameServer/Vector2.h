// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <cmath>

#include "FloatComparison.h"

namespace GameServer
{
	inline class Vector2 operator * (const float& value, const class Vector2& v);

	class Vector2
	{
	public:
		explicit Vector2(const float x = 0.0f, const float y = 0.0f)
		{
			this->x = x;
			this->y = y;
		}

		Vector2 operator + (const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
		Vector2 operator - (const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
		Vector2 operator * (const float& value) const { return Vector2(x * value, y - value); }
		Vector2 operator / (const float& value) const { return Vector2(x / value, y / value); }

		void operator = (const Vector2& v)
		{
			x = v.x;
			y = v.y;
		}

		void operator += (const Vector2& v)
		{
			x += v.x;
			y += v.y;
		}

		void operator -= (const Vector2& v)
		{
			x -= v.x;
			y -= v.y;
		}

		void operator *= (const float& v)
		{
			x *= v;
			y *= v;
		}

		void operator /= (const float& v)
		{
			x /= v;
			y /= v;
		}

		bool operator == (const Vector2& v)
		{
			return FloatComparison::Equal(x, v.x) && FloatComparison::Equal(y, v.y);
		}

		bool operator != (const Vector2& v)
		{
			return !((*this) == v);
		}

		static float Dot(const Vector2& v1, const Vector2& v2)
		{
			return v1.x * v2.x + v1.y * v2.y;
		}

		static float Angle(const Vector2& v1, const Vector2& v2)
		{
			const float dot = Dot(v1, v2);
			const float length = Distance(v1, v2);

			return std::acos(dot / length);
		}

		static float Length(const Vector2& v)
		{
			return std::sqrt(v.x * v.x + v.y * v.y);
		}

		static float Distance(const Vector2& v1, const Vector2& v2)
		{
			const Vector2 v = v1 - v2;
			return Length(v);
		}

		static Vector2 Normalized(const Vector2& v)
		{
			const float length = Length(v);

			return Vector2(v.x / length, v.y / length);
		}

		static Vector2 Project(const Vector2& from, const Vector2& to)
		{
			const float dot = Dot(from, to);
			const float length = Length(to);

			return (dot / (length * length)) * to;
		}

		static const Vector2 Up;
		static const Vector2 Down;
		static const Vector2 Right;
		static const Vector2 Left;

		float x;
		float y;
	};

	Vector2 operator * (const float& value, const Vector2& v) { return v * value; }
}