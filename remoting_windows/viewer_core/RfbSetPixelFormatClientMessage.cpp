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
#include "framework.h"
#include "RfbSetPixelFormatClientMessage.h"


namespace remoting
{
   RfbSetPixelFormatClientMessage::RfbSetPixelFormatClientMessage(const ::innate_subsystem::PixelFormat & pixelFormat)
   {
      m_pixelformat = pixelFormat;
   }

   RfbSetPixelFormatClientMessage::~RfbSetPixelFormatClientMessage()
   {
   }

   /**
    * Pixel Format:
    * 1 - U8  - bits-per-pixel
    * 1 - U8  - depth
    * 1 - U8  - big-endian-flag
    * 1 - U8  - true-color-flag
    * 2 - U16 - red-max
    * 2 - U16 - green-max
    * 2 - U16 - blue-max
    * 1 - U8  - red-shift
    * 1 - U8  - green-shift
    * 1 - U8  - blue-shift
    * 3 -     - padding
    */

   void RfbSetPixelFormatClientMessage::send(RfbOutputGate *output)
   {
      critical_section_lock al(output);
      output->writeUInt8(ClientMsgDefs::SET_PIXEL_FORMAT);
      // padding 3 bytes
      output->writeUInt16(0);
      output->writeUInt8(0);

      // send pixel format
      output->writeUInt8(static_cast<unsigned char>(m_pixelformat.bitsPerPixel));
      output->writeUInt8(static_cast<unsigned char>(m_pixelformat.colorDepth));
      output->writeUInt8(m_pixelformat.bigEndian);
      output->writeUInt8(true); // true color is always true
      output->writeUInt16(m_pixelformat.redMax);
      output->writeUInt16(m_pixelformat.greenMax);
      output->writeUInt16(m_pixelformat.blueMax);
      output->writeUInt8(static_cast<unsigned char>(m_pixelformat.redShift));
      output->writeUInt8(static_cast<unsigned char>(m_pixelformat.greenShift));
      output->writeUInt8(static_cast<unsigned char>(m_pixelformat.blueShift));
      output->writeUInt8(0); // padding bytes (3)
      output->writeUInt8(0);
      output->writeUInt8(0);

      output->flush();
   }

} // namespace remoting



