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


#include "Desktop.h"
#include "UpdateHandler.h"
#include "remoting/remoting/node_config/ConfigReloadListener.h"
#include "UserInput.h"
// External listeners
#include "AbnormDeskTermListener.h"
#include "UpdateSendingListener.h"
#include "subsystem/node/ClipboardListener.h"

namespace remoting_macos
{

   // This class  is a base class CLASS_DECL_REMOTING for different implemetations of desktops
   class CLASS_DECL_REMOTING DesktopBaseImpl :
   virtual public Desktop,
   virtual public ::remoting::UpdateListener,
   virtual public ::subsystem::ClipboardListener,
   virtual public ConfigReloadListener
   {
   public:
      ///DesktopBaseImpl(ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
         //             AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter);
      DesktopBaseImpl();
      ~DesktopBaseImpl() override;


      void initialize_desktop_base_impl(::subsystem::ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
                      AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter);


      // Puts a current desktop name from working session to the
      // desktopName argument and an user name to userMame.
      virtual void getCurrentUserInfo(::string &desktopName, ::string &userName);
      // Puts the current frame buffer dimension and pixel format to
      // the size and pixelformat function arguments.
      virtual void getFramebufferProperties(::int_size & size, ::innate_subsystem::PixelFormat & pixelformat);
      virtual void getPrimaryDesktopCoords(::int_rectangle rectangle);
      virtual void getDisplayNumberCoords(::int_rectangle rectangle, unsigned char dispNumber);
      virtual ::int_rectangle_array_base getDisplaysCoords();
      virtual void getNormalizedRect(::int_rectangle rectangle);
      virtual void getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::int_rectangle rectangle);
      virtual ::operating_system::window getWindowHandleByName(const ::scoped_string &windowName);
      virtual void getApplicationRegion(unsigned int procId, Region & region);
      virtual bool isApplicationInFocus(unsigned int procId);

      virtual void setKeyboardEvent(unsigned int keySym, bool down);
      virtual void setMouseEvent(unsigned short x, unsigned short y, unsigned char buttonMask);
      virtual void setNewClipText(const ::scoped_string &newClipboard);

   //protected:
      // Calling when at least one update has been detected.
      virtual void onUpdate();
      // Implementation of the UpdateRequestListener interface.
      virtual void onUpdateRequest(const ::int_rectangle &rectRequested, bool incremental);
      // Calling when a clipbard change detected.
      virtual void onClipboardUpdate(const ::scoped_string &newClipboard);
      // Calling when a configuration has been reloaded.
      // Uses to update internal settings.
      virtual void onConfigReload(::remoting_node::ServerConfig *serverConfig);
      virtual void applyNewConfiguration() = 0;

      // Returns true when a remote input allowed.
      bool isRemoteInputAllowed();
      // This is an auxiliary function which determines that
      virtual bool isRemoteInputTempBlocked() = 0;

      virtual bool updateExternalFramebuffer(::innate_subsystem::Framebuffer *pframebuffer, const Region & region,
                                             const ::int_rectangle &rectangleViewport);

      void sendUpdate();

      Region m_regionFullReq;
      lockable_critical_section m_criticalsectionReqReg;

      UpdateHandler *m_pupdatehandler;

      // A derived class CLASS_DECL_REMOTING thread control.
      ::happening m_happeningNewUpdate;

      UserInput *m_puserinput;

      // Clipboard
      ::string m_strReceivedClipboard;
      lockable_critical_section m_storedClipCritSec;

      // External listeners
      UpdateSendingListener *m_pupdatesendinglistenerExternal;
      AbnormDeskTermListener *m_pdesktermlistenerExternal;
      ClipboardListener *m_pclipboardlistenerExternal;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting_macos






