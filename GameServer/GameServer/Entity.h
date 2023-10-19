// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <cstdint>

#include <ISerializable.h>
#include <StreamWriter.h>

#include "Vector3.h"

namespace GameServer
{
	class Entity : public BaseServer::ISerializable
	{
	public:
		explicit Entity(Vector3 position) : m_Position(position), m_Moving(false), m_Speed(5.0f) { }
		virtual ~Entity() = default;

		virtual void Update(std::uint32_t timeDifference)
		{
			if (!m_Moving)
				return;

			float deltaTime = timeDifference / 1000.0f;
			float movementDistance = m_Speed * deltaTime;

			float distanceFromOrigin = Vector3::Distance(m_Position, m_Origin);
			float travelDistance = Vector3::Distance(m_Origin, m_Destination);

			if (distanceFromOrigin < travelDistance)
			{
				float distanceToDestination = Vector3::Distance(m_Position, m_Destination);

				if (movementDistance < distanceToDestination)
				{
					m_Position += m_Direction * movementDistance;
				}
				else
				{
					m_Position = m_Destination;
					m_Moving = false;
				}
			}
		}

		void MoveTo(Vector3 position)
		{
			if (m_Position == position)
				return;

			m_Origin = m_Position;
			m_Destination = position;

			m_Direction = m_Destination - m_Origin;
			m_Direction = Vector3::Normalized(m_Direction);

			m_Moving = true;
		}

		Vector3 GetPosition() const { return m_Position; }
		Vector3 GetDestination() const { return m_Destination; }
		float GetSpeed() const { return m_Speed; }

		virtual void Serialize(BaseServer::StreamWriter& streamWriter) const override
		{
			Vector3 position = GetPosition();
			Vector3 destination = GetDestination();

			streamWriter
				<< position.x << position.y << position.z
				<< destination.x << destination.y << destination.z;
		}

		virtual size_t GetSize() const override { return sizeof(float) * 6; }

	private:
		Vector3 m_Position;
		Vector3 m_Destination;

		Vector3 m_Origin;
		Vector3 m_Direction;

		bool m_Moving;

		float m_Speed;

	};
}