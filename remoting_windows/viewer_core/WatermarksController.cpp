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
#include "framework.h"
#include "WatermarksController.h"
#include "watermark_bmp.h"
#include "subsystem/framebuffer/StandardPixelFormatFactory.h"
#include "remoting/remoting/rfb/PixelConverter.h"

namespace remoting
{
   WatermarksController::WatermarksController(void)
   {
   }

   void WatermarksController::setNewFbProperties(const ::int_rectangle &  rectangle, const ::innate_subsystem::PixelFormat & pixelformat)
   {
      setNewPixelFormat(pixelformat);
      setNewFbSize(rectangle);
   }

   void WatermarksController::setNewFbSize(const ::int_rectangle &  rectangle)
   {
      if (m_currentFramebufferRect != rectangle || is_empty())
      {
         m_currentFramebufferRect = rectangle;

         m_currentRect = pframebuffer().getDimension();
         int dx = (rectangle.width()/2) - (m_width/2);
         int dy = (rectangle.height()/2) - (m_height/2);
         m_currentRect.offset(dx, dy);

         m_overlay.setDimension(m_currentRect);
      }
   }

   void WatermarksController::setNewPixelFormat(const ::innate_subsystem::PixelFormat & pixelformat)
   {
      if (is_empty() || m_pframebuffer.getPixelFormat()!= pixelformat)
      {
         ::innate_subsystem::Framebuffer temp;
         ::innate_subsystem::Framebuffer& pframebuffer = pframebuffer(true);

         m_overlay.setPixelFormat(pixelformat);

         if (pixelformat == pframebuffer->getPixelFormat())
            return;

         temp.clone(&pframebuffer);

         pframebuffer->setPixelFormat(pixelformat);

         PixelConverter pc = PixelConverter();
         ::int_rectangle rectangle = pframebuffer->getDimension();

         pc.setPixelFormats(pframebuffer->getPixelFormat(), temp.getPixelFormat());

         pc.convert(rectangle, &pframebuffer, &temp);
      }
   }

   void WatermarksController::showWaterMarks(::innate_subsystem::Framebuffer *pframebuffer, critical_section *fbLock)
   {
      m_overlay.copyFrom(pframebuffer, m_currentRect.left, m_currentRect.top);

      pframebuffer->copyFrom(m_currentRect, &m_pframebuffer, 0, 0);
   }

   void WatermarksController::hideWatermarks(::innate_subsystem::Framebuffer *pframebuffer, critical_section *fbLock)
   {
      pframebuffer->copyFrom(m_currentRect, &m_overlay, 0, 0);
   }

   const ::int_rectangle WatermarksController::CurrentRect()
   {
      return m_currentRect;
   }

   ::innate_subsystem::Framebuffer& WatermarksController::pframebuffer(bool fromFile)
   {
      if (m_pframebuffer.getBuffer() == 0 || fromFile)
      {
         loadFromfile();
      }

      return m_pframebuffer;
   }

   void WatermarksController::loadFromfile()
   {
      int pixelsOfset = *(int*)&WATERMARK_BMP_BODY[14] + 14;

      m_width = *(int*)&WATERMARK_BMP_BODY[18];
      m_height = *(int*)&WATERMARK_BMP_BODY[22];

      int imageBpp = (WATERMARK_BMP_BODY[28] + 7) / 8;

      int pad = 4 - ((m_width * 3) % 4);

      int bufferSize = m_width * m_height * 4;

      unsigned char* buffer = new unsigned char[bufferSize];


      ::int_size size(m_width, m_height);
      ::innate_subsystem::PixelFormat pixelformat = ::innate_subsystem::StandardPixelFormatFactory::create32bppPixelFormat();
      m_pframebuffer.setPropertiesWithoutResize(size, pixelformat);
      m_overlay.setPropertiesWithoutResize(m_overlay.getDimension(), pixelformat);

      for (int i = 0; i < m_height; ++i)
      {
         int vi = m_width * i * 3 + i * pad + pixelsOfset;
         int vb = m_width * (m_height - i - 1) * 4;
         for(int j = 0; j < m_width; ++j)
         {
            int hi = vi + j*3;
            int hb = vb + j*4;

            buffer[hb] = WATERMARK_BMP_BODY[hi];			//B
            buffer[hb + 1] = WATERMARK_BMP_BODY[hi + 1];	//G
            buffer[hb + 2] = WATERMARK_BMP_BODY[hi + 2];	//R
            buffer[hb + 3] = 255;							//A
         }
      }

      m_pframebuffer.setBuffer(buffer);
   }

   bool WatermarksController::is_empty()
   {
      return m_pframebuffer.getBuffer() == 0;
   }
} // namespace remoting