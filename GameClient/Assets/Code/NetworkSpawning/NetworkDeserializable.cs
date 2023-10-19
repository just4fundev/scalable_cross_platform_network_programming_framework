// Copyright Cristian Pagán Díaz. All Rights Reserved.

using System.IO;

namespace NetworkSpawning
{
    public abstract class NetworkDeserializable : NetworkSpawnable
    {
        protected internal virtual void Deserialize(BinaryReader reader) { }
    }
}