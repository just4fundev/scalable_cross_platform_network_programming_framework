// Copyright Cristian Pagán Díaz. All Rights Reserved.

using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace CppPlugins
{
    public class ConnectionManager : MonoBehaviour, IDisposable
    {
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate void OnConnectCallback(bool error, int code, ulong connectionId);

        [DllImport(PluginTarget.CSharpTools)]
        private static extern IntPtr ConnectionManagerConstructor(
            bool activeTCPNoDelay,
            bool tcpNoDelay,
            ulong applicationReceiveBufferSize,
            ulong applicationSendBufferSize,
            bool activeOperatingSystemReceiveBufferSize,
            uint operatingSystemReceiveBufferSize,
            bool activeOperatingSystemSendBufferSize,
            uint operatingSystemSendBufferSize,
            ulong incrementBufferSizeMultiplier,
            ulong threadPoolSize
        );

        [DllImport(PluginTarget.CSharpTools)]
        private static extern void ConnectionManagerDestructor(IntPtr thisPtr);

        [DllImport(PluginTarget.CSharpTools)]
        private static extern void ConnectionManagerStart(IntPtr thisPtr);

        [DllImport(PluginTarget.CSharpTools)]
        private static extern void ConnectionManagerStop(IntPtr thisPtr);

        [DllImport(PluginTarget.CSharpTools)]
        private static extern void ConnectionManagerAsyncConnect(
            IntPtr thisPtr,
            IntPtr[] ipAddresses,
            ulong ipAddressesCount,
            ushort port,
            [MarshalAs(UnmanagedType.FunctionPtr)] OnConnectCallback onConnectCallback
        );

        [DllImport(PluginTarget.CSharpTools)]
        private static extern bool ConnectionManagerSendTo(IntPtr thisPtr, ulong connectionId, IntPtr bytes, ulong size);

        [DllImport(PluginTarget.CSharpTools)]
        private static extern bool ConnectionManagerReceiveFrom(IntPtr thisPtr, ulong connectionId, ref IntPtr bytes, ref ulong size);

        [DllImport(PluginTarget.CSharpTools)]
        private static extern bool ConnectionManagerCloseConnection(IntPtr thisPtr, ulong id);

        [DllImport(PluginTarget.CSharpTools)]
        private static extern bool ConnectionManagerConnectionClosed(IntPtr thisPtr, ulong id);

        [SerializeField] private bool m_ActiveTCPNoDelay = true;
        [SerializeField] private bool m_TCPNoDelay = true;
        [SerializeField] private ulong m_ApplicationReceiveBufferSize = 4096;
        [SerializeField] private ulong m_ApplicationSendBufferSize = 4096;
        [SerializeField] private bool m_ActiveOperatingSystemReceiveBufferSize = true;
        [SerializeField] private uint m_OperatingSystemReceiveBufferSize = 8192;
        [SerializeField] private bool m_ActiveOperatingSystemSendBufferSize = true;
        [SerializeField] private uint m_OperatingSystemSendBufferSize = 8192;
        [SerializeField] private ulong m_IncrementBufferSizeMultiplier = 2;
        [SerializeField] private ulong m_ThreadPoolSize = 1;

        private IntPtr m_ThisPtr;

        private void Awake()
        {
            DontDestroyOnLoad(gameObject);

            m_ThisPtr = ConnectionManagerConstructor(
                m_ActiveTCPNoDelay,
                m_TCPNoDelay,
                m_ApplicationReceiveBufferSize,
                m_ApplicationSendBufferSize,
                m_ActiveOperatingSystemReceiveBufferSize,
                m_OperatingSystemReceiveBufferSize,
                m_ActiveOperatingSystemSendBufferSize,
                m_OperatingSystemSendBufferSize,
                m_IncrementBufferSizeMultiplier,
                m_ThreadPoolSize
            );

            Start();
        }

        private void OnDestroy()
        {
            Stop();
            Dispose();
        }

        public void Dispose()
        {
            ConnectionManagerDestructor(m_ThisPtr);
        }

        public void Start()
        {
            ConnectionManagerStart(m_ThisPtr);
        }

        public void Stop()
        {
            ConnectionManagerStop(m_ThisPtr);
        }

        public void AsyncConnect(string[] ipAddresses, ushort port, OnConnectCallback onConnectCallback)
        {
            IntPtr[] unmanagedPointers = new IntPtr[ipAddresses.Length];
            for (int i = 0; i < ipAddresses.Length; i++)
                unmanagedPointers[i] = Marshal.StringToHGlobalAnsi(ipAddresses[i]);

            ConnectionManagerAsyncConnect(m_ThisPtr, unmanagedPointers, (ulong)unmanagedPointers.Length, port, onConnectCallback);

            for (int i = 0; i < ipAddresses.Length; i++)
                Marshal.FreeHGlobal(unmanagedPointers[i]);
        }

        public bool SendTo(ulong connectionId, OutgoingMessage message)
        {
            message.Free();

            return ConnectionManagerSendTo(m_ThisPtr, connectionId, message.Bytes, message.FullSize);
        }

        public IncomingMessage ReceiveFrom(ulong connectionId)
        {
            IntPtr bytes = IntPtr.Zero;
            ulong size = 0;

            if (!ConnectionManagerReceiveFrom(m_ThisPtr, connectionId, ref bytes, ref size))
                return null;

            return new IncomingMessage(bytes, size);
        }

        public bool CloseConnection(ulong id)
        {
            return ConnectionManagerCloseConnection(m_ThisPtr, id);
        }

        public bool ConnectionClosed(ulong id)
        {
            return ConnectionManagerConnectionClosed(m_ThisPtr, id);
        }
    }
}