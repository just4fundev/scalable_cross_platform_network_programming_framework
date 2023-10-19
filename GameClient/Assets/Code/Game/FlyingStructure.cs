// Copyright Cristian Pagán Díaz. All Rights Reserved.

using System;
using System.Collections;
using UnityEngine;

namespace Game
{
    [RequireComponent(typeof(Renderer))]
    public class FlyingStructure : MonoBehaviour
    {
        private const float k_Speed = 3.0f;

        private Renderer m_Renderer;

        private void Awake()
        {
            m_Renderer = GetComponent<Renderer>();

            enabled = false;
        }

        public void Appear()
        {
            StartCoroutine(AlphaAnimation((v) => v));
        }

        public void Disappear()
        {
            StartCoroutine(AlphaAnimation((v) => 1.0f - v));
        }

        private IEnumerator AlphaAnimation(Func<float, float> operation)
        {
            float time = 0.0f;

            yield return new WaitForEndOfFrame();

            while (time < 1.0f)
            {
                SetAlpha(operation(Curve(time)));

                time += Time.deltaTime * k_Speed;
                yield return new WaitForEndOfFrame();
            }

            SetAlpha(operation(Curve(1.0f)));

            yield return null;
        }

        private void SetAlpha(float alpha)
        {
            Color color = m_Renderer.material.color;
            color.a = alpha;
            m_Renderer.material.color = color;
        }

        private static float Curve(float x)
        {
            return x * x;
        }
    }
}