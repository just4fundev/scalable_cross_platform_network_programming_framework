// Copyright Cristian Pagán Díaz. All Rights Reserved.

using UnityEngine;

namespace Game
{
    public abstract class Status : MonoBehaviour
    {
        private void LateUpdate()
        {
            OnUpdate();
        }

        protected abstract void OnUpdate();
    }
}