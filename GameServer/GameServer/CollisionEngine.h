// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <vector>

#include "Circle.h"
#include "Rect.h"

namespace GameServer
{
	class CollisionEngine final
	{
	public:
		CollisionEngine()
		{
			m_Shapes.reserve(k_MinCapacity);
		}

		~CollisionEngine() = default;

		bool CheckMovement(const Ray& ray)
		{
			return true;
		}

		void AddShape(const IShape* shape)
		{
			m_Shapes.push_back(shape);
		}

		void RemoveShape(const IShape* shape)
		{

		}

	private:
		static constexpr int k_MinCapacity = 100;

		std::vector<const IShape*> m_Shapes;

	};

}