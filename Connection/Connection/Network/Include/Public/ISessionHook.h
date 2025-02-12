#pragma once

#include "Opcode.h"
#include "MessageSize.h"

namespace Connection
{
	class ISessionHook
	{
	public:
		virtual bool IsValidHeader(MessageSize messageSize, Opcode opcode) = 0;
		virtual void ProcessReceivedMessage(class ByteBuffer&& message, class IReceivedQueue& connectoinHook) = 0;

		virtual ~ISessionHook() = default;
		
	};
}