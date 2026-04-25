// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
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
#include "CursorShapeDetector.h"

namespace remoting
{

   const int SLEEP_TIME = 100;

   CursorShapeDetector::CursorShapeDetector()
   {
   }

   CursorShapeDetector::~CursorShapeDetector(void)
   {
      terminate();
      wait();
   }


   void CursorShapeDetector::initialize_cursor_shape_detector(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
                                         CursorShapeGrabber *pcursorshapegrabber, critical_section *mouseGrabLocMut,
                                         ::subsystem::LogWriter * plogwriter)
   {

      initialize_update_detector(pupdatekeeper, pupdatelistener);
      m_pcursorshapegrabber = pcursorshapegrabber;
      m_mouseGrabLocMut = mouseGrabLocMut;
      m_plogwriter = plogwriter;

   }

   void CursorShapeDetector::onTerminate() { m_sleepTimer.set_happening(); }

   void CursorShapeDetector::execute()
   {
      m_plogwriter->information("mouse shape detector thread id = {}", (::iptr) getThreadId());

      while (!isTerminating())
      {
         bool isCursorShapeChanged;
         {
            critical_section_lock al(m_mouseGrabLocMut);
            isCursorShapeChanged = m_pcursorshapegrabber->isCursorShapeChanged();
         }
         if (isCursorShapeChanged)
         {
            m_pupdatekeeper->setCursorShapeChanged();
            doUpdate();
         }
         m_sleepTimer.wait(SLEEP_TIME * 1_ms);
      }
   }


} // namespace remoting



