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
#include "innate_subsystem/framebuffer/Framebuffer.h"
////#include "remoting/remoting/region/::i32_point.h"

#include "remoting/remoting/region/Region.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "subsystem/thread/Thread.h"
//#include "acme/parallelization/happening.h"

#include "acme/parallelization/happening.h"

#include "CursorPainter.h"
#include "WatermarksController.h"

namespace remoting_client
{
   class CoreEventsAdapter;

   class CLASS_DECL_REMOTING FbUpdateNotifier : public ::subsystem::Thread
   {
   public:
      FbUpdateNotifier(::innate_subsystem::Framebuffer *pframebuffer, lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter, WatermarksController* pwatermarkscontroller);
      virtual ~FbUpdateNotifier();

      void setAdapter(CoreEventsAdapter *adapter);

      void onUpdate(const ::i32_rectangle &  rectangle);
      void onPropertiesFb();

      void updatePointerPos(const ::i32_point &pointPosition);
      void setNewCursor(const ::i32_point &pointHotspot,
                        unsigned short width, unsigned short height,
                        const ::array_base<unsigned char> *cursor,
                        const ::array_base<unsigned char> *bitmask);

      void setIgnoreShapeUpdates(bool ignore);
      //protected:
      // Inherited from Thread
      void execute();
      void onTerminate();

      lockable_critical_section * m_pcriticalsectionFramebuffer;
      ::pointer < ::innate_subsystem::Framebuffer > m_pframebuffer;
      CursorPainter m_cursorpainter;

      // Pointer to adapter.
      // Nothing event (changing properties of frame buffer, update frame buffer
      // or update cursor) don't sended to adapter, while m_pcoreeventsadapter is 0.
      ::pointer < CoreEventsAdapter > m_pcoreeventsadapter;

      lockable_critical_section m_criticalsectionUpdate;
      //::subsystem::::happening m_happeningUpdate;
      ::happening m_happeningUpdate;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      //It is used for adding watermarks in demo version.
      ::pointer < WatermarksController > m_pwatermarkscontroller;

      // In this region added all updates of frame buffer and cursor updates.
      ::remoting::Region m_regionUpdate;

      // This rectangle save pointPosition of cursor.
      ::i32_rectangle m_rectangleOldPosition;

      // This flag is true after call onPropertiesFb().
      bool m_bNewSize;

      // This flag is true after set new cursor or update pointPosition.
      bool m_bCursorChange;
      bool m_bGoodCursor;

   private:
      // Do not allow copying objects.
      FbUpdateNotifier(const FbUpdateNotifier &);
      FbUpdateNotifier &operator=(const FbUpdateNotifier &);
   };
} // namespace remoting_client


