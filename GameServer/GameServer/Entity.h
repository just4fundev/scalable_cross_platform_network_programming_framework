#pragma once

#include <cstdint>

#include <ISerializable.h>
#include <StreamWriter.h>

#include "Vector2.h"

namespace GameServer
{
	class Entity : public BaseServer::ISerializable
	{
	public:
		explicit Entity(Vector2 position, const float speed) : m_Position(position), m_Destination(position), m_Moving(false), Speed(speed), m_TravelDistance(0.0f), m_Origin(position), m_Direction() { }
		virtual ~Entity() = default;

		virtual void Update(std::uint32_t timeDifference)
		{
			if (!m_Moving)
				return;

			float deltaTime = timeDifference / 1000.0f;
			float movementDistance = Speed * deltaTime;
			
			Vector2 position = m_Position + m_Direction * movementDistance;

			float distanceFromOrigin = Vector2::Distance(position, m_Origin);
			if (FloatComparison::GreaterEqual(distanceFromOrigin, m_TravelDistance))
			{
				m_Position = m_Destination;
				m_Moving = false;
			}
			else
			{
				m_Position = position;
			}
		}

		void MoveTo(Vector2 destination)
		{
			if (FloatComparison::Less(Vector2::Distance(m_Position, destination), 0.5f))
				return;

			m_Origin = m_Position;
			m_Destination = destination;
			m_TravelDistance = Vector2::Distance(m_Origin, m_Destination);

			m_Direction = (m_Destination - m_Origin) / m_TravelDistance;

			m_Moving = true;
		}

		Vector2 GetPosition() const { return m_Position; }
		Vector2 GetDestination() const { return m_Destination; }

		bool IsMoving() const { return m_Moving; }

		virtual void Serialize(Connection::StreamWriter& streamWriter) const override
		{
			Vector2 position = GetPosition();
			Vector2 destination = GetDestination();

			streamWriter 
				<< static_cast<float>(position.x) 
				<< static_cast<float>(position.y) 
				<< static_cast<float>(destination.x)
				<< static_cast<float>(destination.y);
		}

		virtual size_t GetSize() const override { return sizeof(float) * 4; }

		const float Speed;

	private:
		Vector2 m_Position;
		Vector2 m_Destination;

		Vector2 m_Origin;
		Vector2 m_Direction;

		bool m_Moving;

		float m_TravelDistance;

	};
}