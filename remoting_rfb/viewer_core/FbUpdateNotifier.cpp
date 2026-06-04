// Copyright (C) 2012 GlavSoft LLC.
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
#include "FbupdateNotifier.h"

//#include "subsystem/thread/lockable_critical_section.h"

#include "CoreEventsAdapter.h"

namespace remoting_rfb_client
{
   FbUpdateNotifier::FbUpdateNotifier(::innate_subsystem::Framebuffer *pframebuffer, lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter, WatermarksController* pwatermarkscontroller)
   : m_pframebuffer(pframebuffer),
     m_pcriticalsectionFramebuffer(pcriticalsectionFramebuffer),
     m_plogwriter(plogwriter),
     m_cursorpainter(pframebuffer, plogwriter),
     m_bNewSize(false),
     m_bCursorChange(false),
   m_bGoodCursor(false),
     m_pcoreeventsadapter(0),
     m_pwatermarkscontroller(pwatermarkscontroller)
   {
      m_rectangleOldPosition = m_cursorpainter.hideCursor();

      resumeThread();
   }

   FbUpdateNotifier::~FbUpdateNotifier()
   {
//      try {
//         setThreadToFinish();
//         waitThreadToFinish();
//      } catch (...) {
//      }
   }

void FbUpdateNotifier::destroy()
{
   ::subsystem::Thread::destroy();
//   try {
//      terminateThread();
//      waitThreadToFinish();
//   } catch (...) {
//   }
}

void FbUpdateNotifier::setAdapter(CoreEventsAdapter *adapter)
   {
      critical_section_lock al(&m_criticalsectionUpdate);
      m_pcoreeventsadapter = adapter;
      m_happeningUpdate.set_happening();
   }

   void FbUpdateNotifier::onThreadMain()
   {
      // Don't send any event to adapter, while adapter isn't set.
      {
         bool adapterIsNull = true;
         while (!isThreadTerminating() && adapterIsNull) {
            // Wait event.
            m_happeningUpdate.wait();

            // Check: now adapter is set?
            critical_section_lock al(&m_criticalsectionUpdate);
            if (m_pcoreeventsadapter != 0) {
               adapterIsNull = false;
            }
         }
      }

      // Send event to adapter, while tread isn't terminated.
      while (!isThreadTerminating()) {
         // If flag is set, then thread going to sleep (waitThreadToFinish event).
         bool noUpdates = true;

         // Move updates to local variable with blocking notifier mutex "m_criticalsectionUpdate".
         bool isNewSize;
         bool isCursorChange;
         bool isGoodCursor;
         ::remoting_rfb::Region update;
         {
            critical_section_lock al(&m_criticalsectionUpdate);
            isNewSize = m_bNewSize;
            m_bNewSize = false;

            isCursorChange = m_bCursorChange;
            m_bCursorChange = false;
            isGoodCursor = m_bGoodCursor;
            m_bGoodCursor = false;

            update = m_regionUpdate;
            m_regionUpdate.clear();
         }

         // Send event "Change properties of frame buffer" to adapter
         // with blocking frame buffer mutex "m_criticalsectionFramebuffer".
         if (isNewSize) {
            noUpdates = false;
            m_plogwriter->debug("FbUpdateNotifier (event): new size of frame buffer");
            try {
               critical_section_lock al(m_pcriticalsectionFramebuffer);
               m_pcoreeventsadapter->onFramebufferPropChange(m_pframebuffer);
               // FIXME: it's bad code. Must work without one next line, but not it.
               m_pcoreeventsadapter->onFramebufferUpdate(m_pframebuffer, m_pframebuffer->getDimension());
            } catch (...) {
               m_plogwriter->error("FbUpdateNotifier (event): error in set new size");
            }
         }


         if (isGoodCursor)
         {

            if (m_pcoreeventsadapter)
            {
               m_pcoreeventsadapter->onGoodCursor();
            }

         }
         // Update pointPosition on cursor and send frame buffer update event to adapter
         // with blocking frame buffer mutex "m_criticalsectionFramebuffer".
         if (isCursorChange || !update.is_empty()) {
            noUpdates = false;

            critical_section_lock al(m_pcriticalsectionFramebuffer);
            ::i32_rectangle cursor = m_cursorpainter.showCursor();
            update.addRect(cursor);
            update.addRect(m_rectangleOldPosition);

#ifdef _DEMO_VERSION_
            ::i32_rectangle curWmRect = m_pwatermarkscontroller->CurrentRect();
            Region region(curWmRect);
            region.intersect(&update);
            bool isIntersect = !region.is_empty();
            if (isIntersect)
            {
               m_pwatermarkscontroller->showWaterMarks(m_pframebuffer, m_criticalsectionFramebuffer);
               update.addRect(curWmRect);
            }
#endif

            //::i32_rectangle_array_base rectangleaUpdate;
            auto rectangleaUpdate = update.getRects();
            m_plogwriter->debug("FbUpdateNotifier (event): {} updates", rectangleaUpdate.size());

            try {
               for (::i32_rectangle_array_base::iterator i = rectangleaUpdate.begin(); i != rectangleaUpdate.end(); ++i) {
                  m_pcoreeventsadapter->onFramebufferUpdate(m_pframebuffer, *i);
               }
            } catch (...) {
               m_plogwriter->error("FbUpdateNotifier (event): error in update");
            }


#ifdef _DEMO_VERSION_
            if (isIntersect)
               m_pwatermarkscontroller->hideWatermarks(m_pframebuffer, m_criticalsectionFramebuffer);
#endif

            m_rectangleOldPosition = m_cursorpainter.hideCursor();

         }

         // Pause this thread, if there are no updates (cursor, frame buffer).
         if (noUpdates) {
            m_happeningUpdate.wait();
         }
      }
   }

   void FbUpdateNotifier::onTermThread()
   {
      m_happeningUpdate.set_happening();
   }

   void FbUpdateNotifier::onUpdate(const ::i32_rectangle &  update)
   {
      {
         critical_section_lock al(&m_criticalsectionUpdate);
         m_regionUpdate.addRect(update);
      }
      m_happeningUpdate.set_happening();
      m_plogwriter->debug("FbUpdateNotifier: added rectangle");
   }

   void FbUpdateNotifier::onPropertiesFb()
   {
      {
         critical_section_lock al(&m_criticalsectionUpdate);
         m_regionUpdate.clear();
         m_bNewSize = true;
      }
      m_happeningUpdate.set_happening();
      m_plogwriter->debug("FbUpdateNotifier: new size of frame buffer");
   }

   void FbUpdateNotifier::updatePointerPos(const ::i32_point &pointPosition)
   {
      m_cursorpainter.updatePointerPos(pointPosition);

      critical_section_lock al(&m_criticalsectionUpdate);
      m_bCursorChange = true;
      m_happeningUpdate.set_happening();
   }

   void FbUpdateNotifier::setNewCursor(const ::i32_point &pointHotspot,
                                       ::u16 width, ::u16 height,
                                       const ::array_base<::u8> *cursor,
                                       const ::array_base<::u8> *bitmask)
   {
      {
         critical_section_lock al(m_pcriticalsectionFramebuffer);
         m_cursorpainter.setNewCursor(pointHotspot, width, height, cursor, bitmask);
      }
      critical_section_lock al(&m_criticalsectionUpdate);
      m_bCursorChange = true;
      m_bGoodCursor = true;
      m_happeningUpdate.set_happening();

   }

   void FbUpdateNotifier::setIgnoreShapeUpdates(bool ignore)
   {
      m_cursorpainter.setIgnoreShapeUpdates(ignore);

      critical_section_lock al(&m_criticalsectionUpdate);
      m_bCursorChange = true;
      m_happeningUpdate.set_happening();
   }
} // namespace remoting_rfb_client
