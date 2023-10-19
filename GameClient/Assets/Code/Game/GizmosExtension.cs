// Copyright Cristian Pagán Díaz. All Rights Reserved.

using UnityEngine;

namespace Game
{
    public static class GizmosExtension
    {
        private static Vector3[] SpherePoints = {
        new Vector3( 1.0f,  0.0f,  0.0f),
        new Vector3(-1.0f,  0.0f,  0.0f),
        new Vector3( 0.0f,  1.0f,  0.0f),
        new Vector3( 0.0f, -1.0f,  0.0f),
        new Vector3( 0.0f,  0.0f,  1.0f),
        new Vector3( 0.0f,  0.0f, -1.0f),
    };

        private static Color m_SavedColor;

        public static void SaveColor()
        {
            m_SavedColor = Gizmos.color;
        }

        public static void RestoreColor()
        {
            Gizmos.color = m_SavedColor;
        }

        public static void DrawWireCapsule(Vector3 from, Vector3 to, float radius)
        {
            Gizmos.DrawWireSphere(from, radius);
            Gizmos.DrawWireSphere(to, radius);

            foreach (var item in SpherePoints)
            {
                Vector3 point = item * radius;
                Gizmos.DrawLine(from + point, to + point);
            }
        }
    }
}