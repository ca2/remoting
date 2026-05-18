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

namespace remoting_windows
{


   UpdateHandler::UpdateHandler() {}

   UpdateHandler::~UpdateHandler(void) {}

   void UpdateHandler::initFramebuffer(const ::innate_subsystem::Framebuffer *newFb)
   {
      critical_section_lock al(&m_criticalsectionFramebuffer);
      m_pframebufferBackup->clone(newFb);
   }

   bool UpdateHandler::updateExternalFramebuffer(::innate_subsystem::Framebuffer *pframebuffer, const Region & pregion,
                                                 const ::i32_rectangle &rectangleViewport)
   {
      critical_section_lock al(&m_criticalsectionFramebuffer);
      return updateExternalFramebuffer(pframebuffer, m_pframebufferBackup, pregion, rectangleViewport);
   }

   bool UpdateHandler::updateExternalFramebuffer(::innate_subsystem::Framebuffer *pframebufferTarget,
                                                 ::innate_subsystem::Framebuffer *pframebufferSource, const Region & pregion,
                                                 const ::i32_rectangle &rectangleViewport)
   {
      ::innate_subsystem::PixelFormat pixelformatTarget = pframebufferTarget->getPixelFormat();
      ::innate_subsystem::PixelFormat pixelformatSource = pframebufferSource->getPixelFormat();
      ::i32_size sizeTargetFramebuffer = pframebufferTarget->getDimension();
      ::i32_rectangle rectangleSourceFramebuffer = pframebufferSource->getDimension();
      ::i32_rectangle rectangleResultViewport = rectangleSourceFramebuffer.intersection(rectangleViewport);

      if (pixelformatTarget != pixelformatSource || sizeTargetFramebuffer != rectangleResultViewport.size() ||
          rectangleResultViewport!= rectangleViewport)
      {
         pframebufferTarget->setProperties(rectangleResultViewport, pixelformatSource);
         return false;
      }

      ::int_rectangle_array_base rectanglea;
      ::int_rectangle_array_base::iterator iRect;
      pregion->getRects(rectanglea);

      for (auto & rectangle : rectanglea)
      {
         //auto & rectangle = (*iRect);
         pframebufferTarget->copyFrom(rectangle, pframebufferSource, rectangle.left + rectangleViewport.left, rectangle.top + rectangleViewport.top);
      }
      return true;
   }


} // namespace remoting_windows
