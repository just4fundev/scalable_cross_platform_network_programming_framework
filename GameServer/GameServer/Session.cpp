// Copyright Cristian Pagán Díaz. All Rights Reserved.

#include "Session.h"

#include <StreamReader.h>

using namespace GameServer;

bool Session::Update(std::uint32_t timeDifference, SessionWrapper* sessionWrapper)
{
	bool isOpen = !m_Connection->IsClosed();
	bool result = true;

	Connection::ByteBuffer* byteBuffer = nullptr;
	size_t processedMessagesCount = 0;

	while (processedMessagesCount < m_MaxProccessedPacketsPerSessionUpdate && m_Connection->Receive(byteBuffer))
	{
		processedMessagesCount++;

		BaseServer::StreamReader streamReader(*byteBuffer);

		BaseServer::Opcode opcode;

		try {
			streamReader >> opcode;
		}
		catch (BaseServer::StreamException&) {
			break;
		}

		auto messageHandler = m_MessageHandlerRegistry.GetMessageHandler(opcode);
		if (messageHandler != nullptr)
		{
			try {
				if (!messageHandler(sessionWrapper, streamReader))
				{
					result = false;
					break;
				}
			}
			catch (BaseServer::StreamException&) {
				result = false;
				break;
			}
			catch (Connection::Exception&)
			{
				result = false;
				break;
			}
		}
		else
		{
			break;
		}

		delete byteBuffer;
		byteBuffer = nullptr;
	}

	if (byteBuffer != nullptr)
		delete byteBuffer;

	return result && isOpen;
}