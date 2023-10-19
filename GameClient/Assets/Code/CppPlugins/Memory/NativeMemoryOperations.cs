// Copyright Cristian Pagán Díaz. All Rights Reserved.

using System;
using System.Runtime.InteropServices;

namespace CppPlugins
{
    public static class NativeMemoryOperations
    {
        [DllImport(PluginTarget.CSharpTools)]
        private static extern IntPtr NativeMemoryAllocation(ulong size);

        [DllImport(PluginTarget.CSharpTools)]
        private static extern void NativeMemoryFree(IntPtr ptr);

        public static IntPtr Allocate(ulong size)
        {
            return NativeMemoryAllocation(size);
        }

        public static void Free(IntPtr ptr)
        {
            NativeMemoryFree(ptr);
        }
    }
}