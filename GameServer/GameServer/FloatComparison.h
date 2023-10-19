// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <cfloat>
#include <cmath>

namespace GameServer
{
	class FloatComparison final
	{
	public:
		static inline bool Equal(const float& value, const float& than)
		{
			return NearlyEqual(value, than);
		}

		static inline bool Different(const float& value, const float& than)
		{
			return !Equal(value, than);
		}

		static inline bool Less(const float& value, const float& than)
		{
			return Different(value, than) && value < than;
		}

		static inline bool Greater(const float& value, const float& than)
		{
			return Different(value, than) && value > than;
		}

		static inline bool LessEqual(const float& value, const float& than)
		{
			return Equal(value, than) || value < than;
		}

		static inline bool GreaterEqual(const float& value, const float& than)
		{
			return Equal(value, than) || value > than;
		}

	private:
		static inline bool NearlyEqual(const float a, const float b, const float epsilon = 128 * FLT_EPSILON, const float abs_th = FLT_MIN)
		{
			if (a == b)
				return true;

			const float diff = std::abs(a - b);
			const float norm = std::fmin(std::abs(a) + std::abs(b), FLT_MAX);

			return diff < std::fmax(abs_th, epsilon * norm);
			return true;
		}

	};
}