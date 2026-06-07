// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "CursorPositionDetector.h"


namespace remoting_rfb_windows
{

   const ::i32 MOUSE_SLEEP_TIME = 10;

   // CursorPositionDetector::CursorPositionDetector(::remoting_rfb::UpdateKeeper * pupdatekeeper, ::remoting_rfb::UpdateListener * pupdatelistener,
   //                                                ::subsystem::LogWriter * plogwriter) :
   //     UpdateDetector(pupdatekeeper, pupdatelistener), m_plogwriter = plogwriter;
   // {
   // }
   CursorPositionDetector::CursorPositionDetector()
   {

   }

   CursorPositionDetector::~CursorPositionDetector(void)
   {
      setThreadToFinish();
      waitThreadToFinish();
   }
   void CursorPositionDetector::initialize_cursor_position_detector(::remoting_rfb::UpdateKeeper * pupdatekeeper, ::remoting_rfb::UpdateListener * pupdatelistener,
                                                  ::subsystem::LogWriter * plogwriter)
   {
       initialize_update_detector(pupdatekeeper, pupdatelistener);
      m_plogwriter = plogwriter;

   }

   ::i32_point CursorPositionDetector::getCursorPos() { return m_cursor.getCursorPos(); }

   void CursorPositionDetector::onTermThread() { m_sleepTimer.set_happening(); }

   void CursorPositionDetector::onThreadMain()
   {
      m_plogwriter->information("mouse detector thread id = {}", getThreadId());

      ::i32_point curPoint;

      while (!isThreadTerminating())
      {
         curPoint = m_cursor.getCursorPos();
         if (m_lastCursorPos!= curPoint)
         {
            m_lastCursorPos = curPoint;
            m_pupdatekeeper->setCursorPos(m_lastCursorPos);
            doUpdate();
         }
         m_sleepTimer.waitThreadToFinish(MOUSE_SLEEP_TIME * 1_ms);
      }
   }


} // namespace remoting_rfb_windows



