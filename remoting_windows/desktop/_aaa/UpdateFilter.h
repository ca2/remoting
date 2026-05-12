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

#pragma once


#include "windows/WindowsScreenGrabber.h"
#include "::remoting::ScreenDriver.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "UpdateContainer.h"
#include "GrabOptimizator.h"


namespace remoting_macos
{

   class CLASS_DECL_REMOTING UpdateFilter
   {
   public:
      //UpdateFilter(::remoting::ScreenDriver *screenDriver, ::innate_subsystem::Framebuffer *pframebuffer,
        //           lockable_critical_section *framebufferCriticalSection, ::subsystem::LogWriter * plogwriter);
      UpdateFilter();
      ~UpdateFilter();

      virtual void initialize_update_filter(::remoting::ScreenDriver *screenDriver, ::innate_subsystem::Framebuffer *pframebuffer,
             lockable_critical_section *framebufferCriticalSection, ::subsystem::LogWriter * plogwriter);

      void filter(UpdateContainer & updatecontainer);

   //private:
      void getChangedRegion(::remoting::Region & rgn, const ::int_rectangle & rectangle);
      void updateChangedRect(::remoting::Region & rgn, const ::int_rectangle & rectangle);
      void updateChangedSubRect(::remoting::Region & rgn, const ::int_rectangle & rectangle);

      // This function update the screen grabber frame buffer.
      // If success the function returns the true.
      // Also, this function researching an optimal way to grab from
      // the whole screen grabbing or
      bool grab();

      ::remoting::ScreenDriver *m_pscreendriver;
      ::innate_subsystem::Framebuffer *m_pframebuffer;
      lockable_critical_section *m_pcriticalsectionFramebuffer;
      GrabOptimizator m_grabOptimizator;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };

   //// __UPDATEFILTER_H__


} // namespace remoting_macos
 