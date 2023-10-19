// Copyright Cristian Pagán Díaz. All Rights Reserved.

using UnityEngine;

namespace NetworkSpawning
{
    public abstract class NetworkSpawnable : MonoBehaviour
    {
        protected internal virtual void OnSpawn(object args) { }
    }
}