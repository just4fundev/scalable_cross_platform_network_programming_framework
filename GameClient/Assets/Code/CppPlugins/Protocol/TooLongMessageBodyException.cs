// Copyright Cristian Pagán Díaz. All Rights Reserved.

using System;

namespace CppPlugins
{
    public class TooLongMessageBodyException : Exception
    {
        public TooLongMessageBodyException(string message) : base(message) { }
    }
}