// Copyright Cristian Pagán Díaz. All Rights Reserved.

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