// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "UpdateHandler.h"

namespace remoting
{


   UpdateHandler::UpdateHandler() {}

   UpdateHandler::~UpdateHandler(void) {}

   void UpdateHandler::initFrameBuffer(const ::innate_subsystem::FrameBuffer *newFb)
   {
      critical_section_lock al(&m_fbLocMut);
      m_backupFrameBuffer.clone(newFb);
   }

   bool UpdateHandler::updateExternalFrameBuffer(::innate_subsystem::FrameBuffer *pframebuffer, const Region *pregion,
                                                 const ::int_rectangle &rectangleViewport)
   {
      critical_section_lock al(&m_fbLocMut);
      return updateExternalFrameBuffer(pframebuffer, &m_backupFrameBuffer, pregion, rectangleViewport);
   }

   bool UpdateHandler::updateExternalFrameBuffer(::innate_subsystem::FrameBuffer *pframebufferTarget,
                                                 ::innate_subsystem::FrameBuffer *pframebufferSource, const Region *pregion,
                                                 const ::int_rectangle &rectangleViewport)
   {
      ::innate_subsystem::PixelFormat pixelformatTarget = pframebufferTarget->getPixelFormat();
      ::innate_subsystem::PixelFormat pixelformatSource = pframebufferSource->getPixelFormat();
      ::int_size sizeTargetFramebuffer = pframebufferTarget->getDimension();
      ::int_rectangle rectangleSourceFramebuffer = pframebufferSource->getDimension();
      ::int_rectangle rectangleResultViewport = rectangleSourceFramebuffer.intersection(rectangleViewport);

      if (pixelformatTarget != pixelformatSource || sizeTargetFramebuffer != rectangleResultViewport.size() ||
          rectangleResultViewport!= rectangleViewport)
      {
         pframebufferTarget->setProperties(rectangleResultViewport, pixelformatSource);
         return false;
      }

      ::int_rectangle_array_base rects;
      ::int_rectangle_array_base::iterator iRect;
      pregion->getRectVector(&rects);

      for (iRect = rects.begin(); iRect < rects.end(); iRect++)
      {
         auto & rect = (*iRect);
         pframebufferTarget->copyFrom(rect, pframebufferSource, rect.left + rectangleViewport.left, rect.top + rectangleViewport.top);
      }
      return true;
   }


} // namespace remoting
