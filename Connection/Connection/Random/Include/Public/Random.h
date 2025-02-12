#pragma once

#include <cstdlib>

namespace Connection
{
	class Random
	{
	public:
		static void Seed(unsigned int seed)
		{
			std::srand(seed);
		}

		static int Range(int minInclusive, int maxExclusive)
		{
			return minInclusive + (std::rand() % (maxExclusive - minInclusive));
		}

		static float Range(float minInclusive, float maxInclusive)
		{
			float random = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

			return minInclusive + (random * (maxInclusive - minInclusive));
		}
	};
}