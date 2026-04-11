// Copyright (C) 2015 GlavSoft LLC.
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


#include "acme/subsystem/framebuffer/FrameBuffer.h"
#include "acme/subsystem/framebuffer/PixelFormat.h"
#include "remoting/remoting_common/rfb/StandardPixelFormatFactory.h"
//#include "acme/subsystem/thread/critical_section.h"

namespace remoting
{


   class CLASS_DECL_REMOTING_COMMON WatermarksController
   {
   public:
      WatermarksController(void);

      void setNewFbProperties(const ::int_rectangle &  rect, const ::subsystem::PixelFormat & pf);


      void showWaterMarks(::subsystem::FrameBuffer *frameBuffer,
         critical_section *fbLock);

      void hideWatermarks(::subsystem::FrameBuffer *frameBuffer,
         critical_section *fbLock);

      const ::int_rectangle CurrentRect();

   //private:
      ::int_rectangle m_currentRect;
      ::int_rectangle m_currentFrameBufferRect;

      void setNewPixelFormat(const ::subsystem::PixelFormat & pf);

      void setNewFbSize(const ::int_rectangle &  rect);

      ::subsystem::FrameBuffer m_frameBuffer;
      ::subsystem::FrameBuffer& frameBuffer(bool fromFile = false);
      void loadFromfile();

      bool is_empty();

      ::subsystem::FrameBuffer m_overlay;

      int m_height;
      int m_width;
   };


} // namespace remoting