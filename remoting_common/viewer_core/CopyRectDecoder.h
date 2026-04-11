// Copyright (C) 2011,2012 GlavSoft LLC.
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


#include "DecoderOfRectangle.h"

#include "remoting/remoting_common/region/::int_point.h"
namespace remoting
{
   class CLASS_DECL_REMOTING_COMMON CopyRectDecoder : public DecoderOfRectangle
   {
   public:
      CopyRectDecoder(::subsystem::LogWriter * plogwriter);
      virtual ~CopyRectDecoder();

   protected:
      //
      // This method inherited by DecoderOfRectangle.
      //
      virtual void decode(RfbInputGate *input,
                          ::subsystem::FrameBuffer *frameBuffer,
                          const ::int_rectangle &  dstRect);

      //
      // This method inherited by DecoderOfRectangle.
      //
      virtual void copy(::subsystem::FrameBuffer *dstFrameBuffer,
                        const ::subsystem::FrameBuffer *srcFrameBuffer,
                        const ::int_rectangle &  rect,
                        critical_section *fbLock);

   private:
      // This ::int_point save left-top corner of copy-rectangle.
      ::int_point m_sourcePosition;
   };
} // namespace remoting