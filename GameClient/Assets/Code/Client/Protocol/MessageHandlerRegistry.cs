// Copyright Cristian Pagán Díaz. All Rights Reserved.

namespace Client
{
	namespace Protocol
	{
		public sealed class MessageHandlerRegistry<T>
		{
			private MessageHandler<T>[] m_MessageHandlers;

			public MessageHandlerRegistry(MessageHandler<T>[] messageHandlers)
			{
				m_MessageHandlers = messageHandlers;
			}

            public MessageHandler<T> GetMessageHandler(ushort opcode)
			{
				if (opcode > m_MessageHandlers.Length)
					return null;

				return m_MessageHandlers[opcode];
			}
		};
	}
}