#include "CircleArea.h"
#include "Mathf.h"

using namespace GameServer;

Vector2 CircleArea::GetRandomPoint() const
{
	const float radians = Connection::Random::Range(0.0f, 2.0f * Mathf::Pi);
	const float distance = Connection::Random::Range(0.0f, Radius);
	
	const float x = std::cos(radians) * distance;
	const float y = std::sin(radians) * distance;
	
	return Vector2(x, y) + Position;
}