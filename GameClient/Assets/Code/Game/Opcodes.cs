// Copyright Cristian Pagán Díaz. All Rights Reserved.

using System;

namespace Game
{
    public static class OpcodeExtension
    {
        public const ushort OpcodeSize = sizeof(ushort);

        public static ushort ToValue(this ServerOpcode opcode) { return (ushort)opcode; }

        public static ushort ToValue(this ClientOpcode opcode) { return (ushort)opcode; }

        public static ushort ExtractOpcodeValueFromBytes(byte[] bytes) { return BitConverter.ToUInt16(bytes); }
    }

    public enum ServerOpcode : ushort
    {
        AuthentificationChallenge,
        NewPlayer,
        WorldState,
        Movement,
        Disconnection,
        ServerOpcodeSize
    }

    public enum ClientOpcode : ushort
    {
        AuthentificationChallenge,
        Movement
    }
}