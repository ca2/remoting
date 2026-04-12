// Copyright (C) 2012 GlavSoft LLC.
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
#include "FbupdateNotifier.h"

//#include "subsystem/thread/critical_section.h"

#include "CoreEventsAdapter.h"

namespace remoting
{
   FbUpdateNotifier::FbUpdateNotifier(::subsystem_apex::FrameBuffer *fb, critical_section *fbLock, ::subsystem::LogWriter *logWriter, WatermarksController* wmController)
   : m_frameBuffer(fb),
     m_fbLock(fbLock),
     m_plogwriter(logWriter),
     m_cursorPainter(fb, logWriter),
     m_isNewSize(false),
     m_isCursorChange(false),
   m_isGoodCursor(false),
     m_adapter(0),
     m_watermarksController(wmController)
   {
      m_oldPosition = m_cursorPainter.hideCursor();

      resume();
   }

   FbUpdateNotifier::~FbUpdateNotifier()
   {
      try {
         terminate();
         wait();
      } catch (...) {
      }
   }

   void FbUpdateNotifier::setAdapter(CoreEventsAdapter *adapter)
   {
      critical_section_lock al(&m_updateLock);
      m_adapter = adapter;
      m_eventUpdate.notify();
   }

   void FbUpdateNotifier::execute()
   {
      // Don't send any event to adapter, while adapter isn't set.
      {
         bool adapterIsNull = true;
         while (!isTerminating() && adapterIsNull) {
            // Wait event.
            m_eventUpdate.waitForEvent();

            // Check: now adapter is set?
            critical_section_lock al(&m_updateLock);
            if (m_adapter != 0) {
               adapterIsNull = false;
            }
         }
      }

      // Send event to adapter, while tread isn't terminated.
      while (!isTerminating()) {
         // If flag is set, then thread going to sleep (wait event).
         bool noUpdates = true;

         // Move updates to local variable with blocking notifier mutex "m_updateLock".
         bool isNewSize;
         bool isCursorChange;
         bool isGoodCursor;
         Region update;
         {
            critical_section_lock al(&m_updateLock);
            isNewSize = m_isNewSize;
            m_isNewSize = false;

            isCursorChange = m_isCursorChange;
            m_isCursorChange = false;
            isGoodCursor = m_isGoodCursor;
            m_isGoodCursor = false;

            update = m_update;
            m_update.clear();
         }

         // Send event "Change properties of frame buffer" to adapter
         // with blocking frame buffer mutex "m_fbLock".
         if (isNewSize) {
            noUpdates = false;
            m_plogwriter->debug("FbUpdateNotifier (event): new size of frame buffer");
            try {
               critical_section_lock al(m_fbLock);
               m_adapter->onFrameBufferPropChange(m_frameBuffer);
               // FIXME: it's bad code. Must work without one next line, but not it.
               m_adapter->onFrameBufferUpdate(m_frameBuffer, m_frameBuffer->getDimension());
            } catch (...) {
               m_plogwriter->error("FbUpdateNotifier (event): error in set new size");
            }
         }


         if (isGoodCursor)
         {

            if (m_adapter)
            {
               m_adapter->onGoodCursor();
            }

         }
         // Update position on cursor and send frame buffer update event to adapter
         // with blocking frame buffer mutex "m_fbLock".
         if (isCursorChange || !update.is_empty()) {
            noUpdates = false;

            critical_section_lock al(m_fbLock);
            ::int_rectangle cursor = m_cursorPainter.showCursor();
            update.addRect(cursor);
            update.addRect(m_oldPosition);

#ifdef _DEMO_VERSION_
            ::int_rectangle curWmRect = m_watermarksController->CurrentRect();
            Region reg(curWmRect);
            reg.intersect(&update);
            bool isIntersect = !reg.is_empty();
            if (isIntersect)
            {
               m_watermarksController->showWaterMarks(m_frameBuffer, m_fbLock);
               update.addRect(curWmRect);
            }
#endif

            ::array_base<::int_rectangle> updateList;
            update.getRectVector(&updateList);
            m_plogwriter->debug("FbUpdateNotifier (event): {} updates", updateList.size());

            try {
               for (::array_base<::int_rectangle>::iterator i = updateList.begin(); i != updateList.end(); ++i) {
                  m_adapter->onFrameBufferUpdate(m_frameBuffer, *i);
               }
            } catch (...) {
               m_plogwriter->error("FbUpdateNotifier (event): error in update");
            }


#ifdef _DEMO_VERSION_
            if (isIntersect)
               m_watermarksController->hideWatermarks(m_frameBuffer, m_fbLock);
#endif

            m_oldPosition = m_cursorPainter.hideCursor();

         }

         // Pause this thread, if there are no updates (cursor, frame buffer).
         if (noUpdates) {
            m_eventUpdate.waitForEvent();
         }
      }
   }

   void FbUpdateNotifier::onTerminate()
   {
      m_eventUpdate.notify();
   }

   void FbUpdateNotifier::onUpdate(const ::int_rectangle &  update)
   {
      {
         critical_section_lock al(&m_updateLock);
         m_update.addRect(update);
      }
      m_eventUpdate.notify();
      m_plogwriter->debug("FbUpdateNotifier: added rectangle");
   }

   void FbUpdateNotifier::onPropertiesFb()
   {
      {
         critical_section_lock al(&m_updateLock);
         m_update.clear();
         m_isNewSize = true;
      }
      m_eventUpdate.notify();
      m_plogwriter->debug("FbUpdateNotifier: new size of frame buffer");
   }

   void FbUpdateNotifier::updatePointerPos(const ::int_point *position)
   {
      m_cursorPainter.updatePointerPos(position);

      critical_section_lock al(&m_updateLock);
      m_isCursorChange = true;
      m_eventUpdate.notify();
   }

   void FbUpdateNotifier::setNewCursor(const ::int_point *hotSpot,
                                       unsigned short width, unsigned short height,
                                       const ::array_base<unsigned char> *cursor,
                                       const ::array_base<unsigned char> *bitmask)
   {
      {
         critical_section_lock al(m_fbLock);
         m_cursorPainter.setNewCursor(hotSpot, width, height, cursor, bitmask);
      }
      critical_section_lock al(&m_updateLock);
      m_isCursorChange = true;
      m_isGoodCursor = true;
      m_eventUpdate.notify();

   }

   void FbUpdateNotifier::setIgnoreShapeUpdates(bool ignore)
   {
      m_cursorPainter.setIgnoreShapeUpdates(ignore);

      critical_section_lock al(&m_updateLock);
      m_isCursorChange = true;
      m_eventUpdate.notify();
   }
} // namespace remoting