// Copyright Cristian Pagán Díaz. All Rights Reserved.

using System;
using System.IO;

namespace CppPlugins
{
    public class OutgoingMessage : IDisposable
    {
        internal IntPtr Bytes { get; private set; }

        public ushort FullSize { get; private set; }

        public BinaryWriter BinaryWriter { get; private set; }

        private UnmanagedMemoryStream m_MemoryStream;

        private bool m_Freed;

        public OutgoingMessage(ushort bodySize)
        {
            m_Freed = false;

            ushort headerSize = sizeof(ushort);

            ushort maxBodySize = (ushort)(ushort.MaxValue - headerSize);
            if (bodySize > maxBodySize)
                throw new TooLongMessageBodyException("The body size of the message (" + bodySize + ") should be less than " + maxBodySize);

            FullSize = (ushort)(headerSize + bodySize);

            Bytes = NativeMemoryOperations.Allocate(FullSize);

            unsafe {
                byte* bytePtr = (byte*)Bytes.ToPointer();
                m_MemoryStream = new UnmanagedMemoryStream(bytePtr, FullSize, FullSize, FileAccess.Write);
            }

            BinaryWriter = new BinaryWriter(m_MemoryStream);

            BinaryWriter.Write(bodySize);
        }

        internal void Free() { m_Freed = true; }

        public void Dispose()
        {
            if (!m_Freed)
            {
                NativeMemoryOperations.Free(Bytes);
                m_Freed = true;
            }

            m_MemoryStream.Dispose();
            BinaryWriter.Dispose();
        }
    }
}