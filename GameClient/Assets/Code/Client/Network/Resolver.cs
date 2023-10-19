// Copyright Cristian Pagán Díaz. All Rights Reserved.

using CppPlugins;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

namespace Client
{
    namespace Network
    {
        public class Resolver : MonoBehaviour
        {
            [SerializeField] private ConnectionManager m_ConnectionManager;
            [SerializeField] private string m_Hostname;
            [SerializeField] private ushort m_Port;

            private readonly object mMutex = new object();

            private Thread mThread;
            private string[] mIpAddresses;

            private void Start()
            {
                lock (mMutex) {
                    mIpAddresses = null;
                }

                mThread = new Thread(() => {
                    LinkedList<string> ipAddresses = new LinkedList<string>();
                    IPHostEntry ipHostEntry = Dns.GetHostEntry(m_Hostname);

                    foreach (IPAddress ipAddress in ipHostEntry.AddressList)
                    {
                        if (ipAddress.AddressFamily == AddressFamily.InterNetwork)
                            ipAddresses.AddLast(ipAddress.ToString());
                    }

                    lock (mMutex) {
                        if (ipAddresses.Count > 0)
                        {
                            mIpAddresses = new string[ipAddresses.Count];

                            int index = 0;
                            foreach (string ipAddress in ipAddresses)
                            {
                                char[] ipAddressCharacters = ipAddress.ToCharArray();
                                char[] characters = new char[ipAddressCharacters.Length];

                                for (int i = 0; i < ipAddressCharacters.Length; i++)
                                    characters[i] = ipAddressCharacters[i];

                                mIpAddresses[index] = new string(characters);

                                index++;
                            }
                        }
                    }
                });

                mThread.Start();
            }

            private void Update()
            {
                if (mThread.IsAlive)
                    return;

                string[] ipAddresses = null;

                lock (mMutex) {
                    if (mIpAddresses != null)
                    {
                        ipAddresses = new string[mIpAddresses.Length];

                        int index = 0;
                        foreach (string ipAddress in mIpAddresses)
                        {
                            char[] ipAddressCharacters = ipAddress.ToCharArray();
                            char[] characters = new char[ipAddressCharacters.Length];

                            for (int i = 0; i < ipAddressCharacters.Length; i++)
                                characters[i] = ipAddressCharacters[i];

                            ipAddresses[index] = new string(characters);

                            index++;
                        }
                    }
                }

                if (ipAddresses != null)
                {
                    Connector.Instantiate(string.Format("Connector[{0}:{1}]", m_Hostname, m_Port), m_ConnectionManager, ipAddresses, m_Port);
                }
                else
                {
                    
                }

                Destroy(gameObject);
            }
        }
    }
}