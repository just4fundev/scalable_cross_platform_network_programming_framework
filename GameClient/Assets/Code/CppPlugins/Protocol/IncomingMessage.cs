// Copyright Cristian Pagán Díaz. All Rights Reserved.

using System;
using System.IO;

namespace CppPlugins
{
    public class IncomingMessage : IDisposable
    {
        public BinaryReader BinaryReader { get; private set; }

        private UnmanagedMemoryStream m_MemoryStream;

        private IntPtr m_Bytes;

        internal IncomingMessage(IntPtr bytes, ulong size)
        {
            m_Bytes = bytes;

            unsafe {
                byte* bytePtr = (byte*)m_Bytes.ToPointer();
                m_MemoryStream = new UnmanagedMemoryStream(bytePtr, (long)size, (long)size, FileAccess.Read);
            }

            BinaryReader = new BinaryReader(m_MemoryStream);
        }

        public void Dispose()
        {
            NativeMemoryOperations.Free(m_Bytes);

            m_MemoryStream.Close();
            BinaryReader.Close();
        }
    }
}