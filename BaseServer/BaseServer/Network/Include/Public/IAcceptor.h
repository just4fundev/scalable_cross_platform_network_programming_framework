#pragma once

namespace BaseServer
{
	class IAcceptor
	{
	public:
		virtual void Start() = 0;
		virtual void Close() = 0;
		virtual void SubscribeObserver(class IAcceptorObserver* observer) = 0;
		virtual void UnsubscribeObserver(class IAcceptorObserver* observer) = 0;

		virtual ~IAcceptor() = default;
	};
}