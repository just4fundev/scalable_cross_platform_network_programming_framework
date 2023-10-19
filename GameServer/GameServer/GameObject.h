// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <vector>

#include "IComponent.h"

namespace GameServer
{
	class GameObject final
	{
	public:
		GameObject()
		{
			m_Components.reserve(k_MinCapacity);
		}

		~GameObject()
		{
			for (IComponent* component : m_Components)
				delete component;
		}

		void AddComponent(IComponent* component)
		{
			m_Components.push_back(component);
		}

		void Update(float deltaTime)
		{
			for (IComponent* component : m_Components)
				component->OnUpdate(deltaTime);
		}

	private:
		static constexpr int k_MinCapacity = 5;

		std::vector<IComponent*> m_Components;
	};
}