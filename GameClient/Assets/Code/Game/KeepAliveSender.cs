// Copyright Cristian Pagán Díaz. All Rights Reserved.

namespace Game
{
    public class KeepAliveSender : TimerEvent
    {
        protected override void Event()
        {
            m_PlayerController.SendKeepAlive();
        }
    }
}