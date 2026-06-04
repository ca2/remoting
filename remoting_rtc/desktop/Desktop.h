// Copyright (C) 2011,2012 GlavSoft LLC.
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


//////#include "subsystem/platform/::string.h"

#include "remoting/remoting_rtc/region/Region.h"
#include "subsystem/framebuffer/PixelFormat.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "remoting/remoting_rtc/framebuffer_update_sender/UpdateRequestListener.h"
//#include aaa_<vector>

namespace remoting
{

   // This class  is a public interface to a desktop.
   class Desktop : public ::remoting_rtc::UpdateRequestListener
   {
   public:
      //virtual ~Desktop() {}


      virtual void initialize_desktop(
         ::remoting_rtc_node::Configurator * pconfigurator,
         ::subsystem::ClipboardListener *pclipboardlistenerExternal,
         UpdateSendingListener *pupdatesendinglistenerExternal,
         AbnormDeskTermListener *pdesktermlistenerExternal,
         ::subsystem::LogWriter * plogwriter) = 0;

      // Puts a current desktop name from working session to the
      // desktopName argument and an user name to userMame.
      virtual void getCurrentUserInfo(::string &desktopName, ::string &userName) = 0;
      // Puts the current frame buffer dimension and pixel format to
      // the size and pixelformat function arguments.
      virtual void getFramebufferProperties(::i32_size & size, ::innate_subsystem::PixelFormat & pixelformat) = 0;

      virtual void getPrimaryDesktopCoords(::i32_rectangle rectangle) = 0;
      // Returns a rectangle that is normilized from "virtual desktop" to frame buffer coordinates.
      virtual void getNormalizedRect(::i32_rectangle rectangle) = 0;
      virtual void getDisplayNumberCoords(::i32_rectangle rectangle, ::u8 dispNumber) = 0;
      virtual ::i32_rectangle_array_base getDisplaysCoords() = 0;
      virtual void getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle rectangle) = 0;
      virtual ::operating_system::window getWindowHandleByName(const ::scoped_string &windowName) = 0;

      virtual void getApplicationRegion(::u32 procId, ::remoting_rtc::Region & region) = 0;
      virtual bool isApplicationInFocus(::u32 procId) = 0;

      virtual void setKeyboardEvent(::u32 keySym, bool down) = 0;
      virtual void setMouseEvent(::u16 x, ::u16 y, ::u8 buttonMask) = 0;
      virtual void setNewClipText(const ::scoped_string &newClipboard) = 0;

      // Updates external frame buffer pixels only for the region from view port
      // located at the place in a central frame buffer.
      // If view port is out of central frame buffer bounds the function will return false.
      virtual bool updateExternalFramebuffer(::innate_subsystem::Framebuffer *pframebuffer, const ::remoting_rtc::Region & region,
                                             const ::i32_rectangle &rectangleViewport) = 0;
   };


} // namespace remoting







