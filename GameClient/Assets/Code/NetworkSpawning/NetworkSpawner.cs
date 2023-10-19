// Copyright Cristian Pagán Díaz. All Rights Reserved.

using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

namespace NetworkSpawning
{
    public struct NetworkResource
    {
        public string Name { get; private set; }
        public string Path { get; private set; }

        public NetworkResource(string name, string path)
        {
            Name = name;
            Path = path;
        }
    }

    public class NetworkSpawner
    {
        private Dictionary<Type, NetworkResource> m_NetworkResources;

        public NetworkSpawner(Dictionary<Type, NetworkResource> networkResources)
        {
            m_NetworkResources = networkResources;
        }

        public T Spawn<T>(BinaryReader reader, object args) where T : NetworkDeserializable
        {
            Type type = typeof(T);
            NetworkResource networkResource = m_NetworkResources[type];

            GameObject newGameObject = UnityEngine.Object.Instantiate((GameObject)Resources.Load(networkResource.Path));
            newGameObject.name = networkResource.Name;

            T spawnedComponent = newGameObject.GetComponent<T>();
            spawnedComponent.OnSpawn(args);
            spawnedComponent.Deserialize(reader);

            return spawnedComponent;
        }
    }
}