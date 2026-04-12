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

#pragma once


//#include "log_writer/LogWriter.h"
#include "innate_subsystem/framebuffer/FrameBuffer.h"
//#include "remoting/remoting_common/region/::int_point.h"

#include "remoting/remoting_common/region/Region.h"
//#include "subsystem/thread/critical_section.h"
#include "subsystem/thread/Thread.h"
#include "subsystem/node/WindowsEvent.h"

#include "acme/parallelization/happening.h"

#include "CursorPainter.h"
#include "WatermarksController.h"

namespace remoting
{
   class CoreEventsAdapter;

   class CLASS_DECL_REMOTING_COMMON FbUpdateNotifier : public ::subsystem::Thread
   {
   public:
      FbUpdateNotifier(::innate_subsystem::FrameBuffer *fb, critical_section *fbLock, ::subsystem::LogWriter * plogwriter, WatermarksController* wmController);
      virtual ~FbUpdateNotifier();

      void setAdapter(CoreEventsAdapter *adapter);

      void onUpdate(const ::int_rectangle &  rect);
      void onPropertiesFb();

      void updatePointerPos(const ::int_point *position);
      void setNewCursor(const ::int_point *hotSpot,
                        unsigned short width, unsigned short height,
                        const ::array_base<unsigned char> *cursor,
                        const ::array_base<unsigned char> *bitmask);

      void setIgnoreShapeUpdates(bool ignore);
      //protected:
      // Inherited from Thread
      void execute();
      void onTerminate();

      critical_section *m_fbLock;
      ::innate_subsystem::FrameBuffer *m_frameBuffer;
      CursorPainter m_cursorPainter;

      // Pointer to adapter.
      // Nothing event (changing properties of frame buffer, update frame buffer
      // or update cursor) don't sended to adapter, while m_adapter is 0.
      CoreEventsAdapter *m_adapter;

      critical_section m_updateLock;
      //::subsystem::WindowsEvent m_eventUpdate;
      ::happening m_eventUpdate;

      ::subsystem::LogWriter *m_plogwriter;

      //It is used for adding watermarks in demo version.
      WatermarksController* m_watermarksController;

      // In this region added all updates of frame buffer and cursor updates.
      Region m_update;

      // This rectangle save position of cursor.
      ::int_rectangle m_oldPosition;

      // This flag is true after call onPropertiesFb().
      bool m_isNewSize;

      // This flag is true after set new cursor or update position.
      bool m_isCursorChange;
      bool m_isGoodCursor;

   private:
      // Do not allow copying objects.
      FbUpdateNotifier(const FbUpdateNotifier &);
      FbUpdateNotifier &operator=(const FbUpdateNotifier &);
   };
} // namespace remoting


