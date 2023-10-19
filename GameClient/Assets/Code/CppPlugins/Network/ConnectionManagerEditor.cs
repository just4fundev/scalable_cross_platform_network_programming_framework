// Copyright Cristian Pagán Díaz. All Rights Reserved.

using UnityEditor;
using UnityEngine;

namespace CppPlugins
{
#if UNITY_EDITOR
    [CustomEditor(typeof(ConnectionManager))]
#endif
    public class ConnectionManagerEditor
#if UNITY_EDITOR
        : Editor
#endif
    {
#if UNITY_EDITOR
        private SerializedProperty m_ActiveTCPNoDelayProperty;
        private SerializedProperty m_TCPNoDelayProperty;
        private SerializedProperty m_ApplicationReceiveBufferSizeProperty;
        private SerializedProperty m_ApplicationSendBufferSizeProperty;
        private SerializedProperty m_ActiveOperatingSystemReceiveBufferSizeProperty;
        private SerializedProperty m_OperatingSystemReceiveBufferSizeProperty;
        private SerializedProperty m_ActiveOperatingSystemSendBufferSize;
        private SerializedProperty m_OperatingSystemSendBufferSizeProperty;
        private SerializedProperty m_IncrementBufferSizeMultiplierProperty;
        private SerializedProperty m_ThreadPoolSizeProperty;

        void OnEnable()
        {
            m_ActiveTCPNoDelayProperty = serializedObject.FindProperty("m_ActiveTCPNoDelay");
            m_TCPNoDelayProperty = serializedObject.FindProperty("m_TCPNoDelay");
            m_ApplicationReceiveBufferSizeProperty = serializedObject.FindProperty("m_ApplicationReceiveBufferSize");
            m_ApplicationSendBufferSizeProperty = serializedObject.FindProperty("m_ApplicationSendBufferSize");
            m_ActiveOperatingSystemReceiveBufferSizeProperty = serializedObject.FindProperty("m_ActiveOperatingSystemReceiveBufferSize");
            m_OperatingSystemReceiveBufferSizeProperty = serializedObject.FindProperty("m_OperatingSystemReceiveBufferSize");
            m_ActiveOperatingSystemSendBufferSize = serializedObject.FindProperty("m_ActiveOperatingSystemSendBufferSize");
            m_OperatingSystemSendBufferSizeProperty = serializedObject.FindProperty("m_OperatingSystemSendBufferSize");
            m_IncrementBufferSizeMultiplierProperty = serializedObject.FindProperty("m_IncrementBufferSizeMultiplier");
            m_ThreadPoolSizeProperty = serializedObject.FindProperty("m_ThreadPoolSize");
        }

        public override void OnInspectorGUI()
        {

            serializedObject.Update();

            GUILayout.BeginHorizontal();
            EditorGUILayout.PropertyField(m_ActiveTCPNoDelayProperty);
            if (m_ActiveTCPNoDelayProperty.boolValue)
                EditorGUILayout.PropertyField(m_TCPNoDelayProperty);
            GUILayout.EndHorizontal();

            EditorGUILayout.PropertyField(m_ApplicationReceiveBufferSizeProperty);
            EditorGUILayout.PropertyField(m_ApplicationSendBufferSizeProperty);

            GUILayout.BeginHorizontal();
            EditorGUILayout.PropertyField(m_ActiveOperatingSystemReceiveBufferSizeProperty);
            if (m_ActiveOperatingSystemReceiveBufferSizeProperty.boolValue)
                EditorGUILayout.PropertyField(m_OperatingSystemReceiveBufferSizeProperty);
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            EditorGUILayout.PropertyField(m_ActiveOperatingSystemSendBufferSize);
            if (m_ActiveOperatingSystemSendBufferSize.boolValue)
                EditorGUILayout.PropertyField(m_OperatingSystemSendBufferSizeProperty);
            GUILayout.EndHorizontal();

            EditorGUILayout.PropertyField(m_IncrementBufferSizeMultiplierProperty);
            EditorGUILayout.PropertyField(m_ThreadPoolSizeProperty);

            serializedObject.ApplyModifiedProperties();
        }
#endif
    }
}