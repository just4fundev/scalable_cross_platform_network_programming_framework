#include "Session.h"

#include <StreamReader.h>
#include <PreprocessedMessage.h>

using namespace GameServer;

bool Session::Update(std::uint32_t timeDifference, SessionWrapper* sessionWrapper)
{
	bool isOpen = !m_Connection->IsClosed();
	bool result = true;

	Connection::PreprocessedMessage* preprocessedMessage = nullptr;
	size_t processedMessagesCount = 0;

	while (processedMessagesCount < m_MaxProccessedPacketsPerSessionUpdate && m_Connection->Receive(preprocessedMessage))
	{
		processedMessagesCount++;

		Connection::StreamReader dataReader(preprocessedMessage->Data);

		Connection::Opcode opcode;
		dataReader >> opcode;

		auto messageHandler = m_MessageHandlerRegistry.GetMessageHandler(opcode);
		try {
			if (!messageHandler(sessionWrapper, dataReader, preprocessedMessage->Metadata))
			{
				result = false;
				break;
			}
		}
		catch (Connection::StreamException&) {
		}

		delete preprocessedMessage;
		preprocessedMessage = nullptr;
	}

	if (preprocessedMessage != nullptr)
		delete preprocessedMessage;

	return result && isOpen;
}