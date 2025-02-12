#pragma once

namespace Connection
{
	class IReceivedQueue
	{
	public:
		virtual void EnqueueReceivedMessage(class PreprocessedMessage&& sendingMessage) = 0;

		virtual ~IReceivedQueue() = default;
		
	};
}