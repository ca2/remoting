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

#pragma once


#include "innate_subsystem/framebuffer/Framebuffer.h"
//#include "subsystem/framebuffer/PixelFormat.h"
//#include "subsystem/framebuffer/StandardPixelFormatFactory.h"
//#include "subsystem/thread/lockable_critical_section.h"

namespace remoting_client
{


   class CLASS_DECL_REMOTING WatermarksController :
      virtual public ::particle
   {
   public:


      ::pointer < ::innate_subsystem::Framebuffer>m_pframebuffer;
      ::pointer < ::innate_subsystem::Framebuffer> m_pframebufferOverlay;
      ::i32_rectangle m_currentRect;
      ::i32_rectangle m_currentFramebufferRect;


      ::i32 m_height;
      ::i32 m_width;

      WatermarksController(void);

      void setNewFbProperties(const ::i32_rectangle &  rectangle, const ::innate_subsystem::PixelFormat & pixelformat);


      void showWaterMarks(::innate_subsystem::Framebuffer *pframebuffer,
         lockable_critical_section *pcriticalsectionFramebuffer);

      void hideWatermarks(::innate_subsystem::Framebuffer *pframebuffer,
         lockable_critical_section *pcriticalsectionFramebuffer);

      const ::i32_rectangle CurrentRect();

   //private:
      void setNewPixelFormat(const ::innate_subsystem::PixelFormat & pixelformat);

      void setNewFbSize(const ::i32_rectangle &  rectangle);


      ::innate_subsystem::Framebuffer * framebuffer(bool fromFile = false);
      void loadFromfile();

      bool is_empty();

   };


} // namespace remoting_client