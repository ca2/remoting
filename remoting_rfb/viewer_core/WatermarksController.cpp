// Copyright (C) 2015 GlavSoft LLC.
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
#include "WatermarksController.h"
#include "watermark_bmp.h"
#include "subsystem/framebuffer/StandardPixelFormatFactory.h"
#include "remoting/remoting_rfb/rfb/PixelConverter.h"

namespace remoting_rfb_client
{
   WatermarksController::WatermarksController(void)
   {
   }

   void WatermarksController::setNewFbProperties(const ::i32_rectangle &  rectangle, const ::innate_subsystem::PixelFormat & pixelformat)
   {
      setNewPixelFormat(pixelformat);
      setNewFbSize(rectangle);
   }

   void WatermarksController::setNewFbSize(const ::i32_rectangle &  rectangle)
   {
      if (m_currentFramebufferRect != rectangle || is_empty())
      {
         m_currentFramebufferRect = rectangle;

         m_currentRect = framebuffer()->getDimension();
         ::i32 dx = (rectangle.width()/2) - (m_width/2);
         ::i32 dy = (rectangle.height()/2) - (m_height/2);
         m_currentRect.offset(dx, dy);

         m_pframebufferOverlay->setDimension(m_currentRect);
      }
   }

   void WatermarksController::setNewPixelFormat(const ::innate_subsystem::PixelFormat & pixelformat)
   {
      if (is_empty() || m_pframebuffer->getPixelFormat()!= pixelformat)
      {
         ::innate_subsystem::Framebuffer temp;
         auto pframebuffer = framebuffer(true);

         m_pframebufferOverlay->setPixelFormat(pixelformat);

         if (pixelformat == pframebuffer->getPixelFormat())
            return;

         temp.clone(pframebuffer);

         pframebuffer->setPixelFormat(pixelformat);

         //::remoting_rfb::PixelConverter pixelconverter = PixelConverter();
         ::remoting_rfb::PixelConverter pixelconverter;
         ::i32_rectangle rectangle = pframebuffer->getDimension();

         pixelconverter.setPixelFormats(pframebuffer->getPixelFormat(), temp.getPixelFormat());

         pixelconverter.convert(rectangle, pframebuffer, &temp);
      }
   }

   void WatermarksController::showWaterMarks(::innate_subsystem::Framebuffer *pframebuffer, lockable_critical_section *pcriticalsectionFramebuffer)
   {
      m_pframebufferOverlay->copyFrom(pframebuffer, m_currentRect.left, m_currentRect.top);

      pframebuffer->copyFrom(m_currentRect, m_pframebuffer, 0, 0);
   }

   void WatermarksController::hideWatermarks(::innate_subsystem::Framebuffer *pframebuffer, lockable_critical_section *pcriticalsectionFramebuffer)
   {
      pframebuffer->copyFrom(m_currentRect, m_pframebufferOverlay, 0, 0);
   }

   const ::i32_rectangle WatermarksController::CurrentRect()
   {
      return m_currentRect;
   }

   ::innate_subsystem::Framebuffer * WatermarksController::framebuffer(bool fromFile)
   {
      if (m_pframebuffer->getBuffer() == 0 || fromFile)
      {
         loadFromfile();
      }

      return m_pframebuffer;
   }

   void WatermarksController::loadFromfile()
   {
      ::i32 pixelsOfset = *(::i32*)&WATERMARK_BMP_BODY[14] + 14;

      m_width = *(::i32*)&WATERMARK_BMP_BODY[18];
      m_height = *(::i32*)&WATERMARK_BMP_BODY[22];

      ::i32 imageBpp = (WATERMARK_BMP_BODY[28] + 7) / 8;

      ::i32 pad = 4 - ((m_width * 3) % 4);

      ::i32 bufferSize = m_width * m_height * 4;

      ::u8* buffer = new ::u8[bufferSize];


      ::i32_size size(m_width, m_height);
      ::innate_subsystem::PixelFormat pixelformat = ::innate_subsystem::StandardPixelFormatFactory::create32bppPixelFormat();
      m_pframebuffer->setPropertiesWithoutResize(size, pixelformat);
      m_pframebufferOverlay->setPropertiesWithoutResize(m_pframebufferOverlay->getDimension(), pixelformat);

      for (::i32 i = 0; i < m_height; ++i)
      {
         ::i32 vi = m_width * i * 3 + i * pad + pixelsOfset;
         ::i32 vb = m_width * (m_height - i - 1) * 4;
         for(::i32 j = 0; j < m_width; ++j)
         {
            ::i32 hi = vi + j*3;
            ::i32 hb = vb + j*4;

            buffer[hb] = WATERMARK_BMP_BODY[hi];			//B
            buffer[hb + 1] = WATERMARK_BMP_BODY[hi + 1];	//G
            buffer[hb + 2] = WATERMARK_BMP_BODY[hi + 2];	//R
            buffer[hb + 3] = 255;							//A
         }
      }

      m_pframebuffer->setBuffer(buffer);
   }

   bool WatermarksController::is_empty()
   {
      return m_pframebuffer->getBuffer() == 0;
   }
} // namespace remoting_rfb_client