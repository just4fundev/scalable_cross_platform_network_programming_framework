#pragma once

#include <iostream>

#include "MessageHandler.h"

namespace BaseServer
{
	template<typename T> class MessageHandlerRegistry final
	{
	public:
		MessageHandlerRegistry(const MessageHandler<T> * const messageHandlers, const size_t size) : 
			m_MessageHandlers(messageHandlers), 
			m_Size(size) { }
		
		~MessageHandlerRegistry() = default;

		MessageHandler<T> GetMessageHandler(std::uint16_t opcode) const
		{ 
			if (opcode > m_Size)
				return nullptr;

			return m_MessageHandlers[opcode]; 
		}

	private:
		const size_t m_Size;
		const MessageHandler<T> * const m_MessageHandlers;
	};
}