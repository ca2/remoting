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


#include "Desktop.h"
#include "UpdateHandler.h"
#include "remoting/remoting_rfb/node_config/ConfigReloadListener.h"
#include "UserInput.h"
// External listeners
#include "AbnormDeskTermListener.h"
#include "UpdateSendingListener.h"
#include "subsystem/node/ClipboardListener.h"
#include "acme/parallelization/happening.h"

namespace remoting_rfb
{

   // This class  is a base class CLASS_DECL_REMOTING_RFB for different implemetations of desktops
   class CLASS_DECL_REMOTING_RFB DesktopBaseImpl :
   virtual public Desktop,
   virtual public UpdateListener,
   virtual public ::subsystem::ClipboardListener,
   virtual public ::remoting_rfb_node::ConfigReloadListener
   {
   public:


      Region m_regionFullReq;
      lockable_critical_section m_criticalsectionReqReg;

      ::pointer < UpdateHandler > m_pupdatehandler;

      // A derived class CLASS_DECL_REMOTING_RFB thread control.
      ::happening m_happeningNewUpdate;

      ::pointer < UserInput > m_puserinput;

      // Clipboard
      ::string m_strReceivedClipboard;
      lockable_critical_section m_storedClipCritSec;

      // External listeners
      ::pointer < UpdateSendingListener > m_pupdatesendinglistenerExternal;
      ::pointer < AbnormDeskTermListener > m_pdesktermlistenerExternal;
      ::pointer < ClipboardListener >     m_pclipboardlistenerExternal;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;



      ///DesktopBaseImpl(ClipboardListener *pclipboardlistenerExternal, UpdateSendingListener *pupdatesendinglistenerExternal,
         //             AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter);
      DesktopBaseImpl();
      ~DesktopBaseImpl() override;


      void initialize_desktop_base_impl(::remoting_rfb_node::Configurator * pconfigurator,
         ::subsystem::ClipboardListener *pclipboardlistenerExternal,
         UpdateSendingListener *pupdatesendinglistenerExternal,
                      AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter);


      // Puts a current desktop name from working session to the
      // desktopName argument and an user name to userMame.
      void getCurrentUserInfo(::string &desktopName, ::string &userName) override;
      // Puts the current frame buffer dimension and pixel format to
      // the size and pixelformat function arguments.
      void getFramebufferProperties(::i32_size & size, ::innate_subsystem::PixelFormat & pixelformat) override;
      void getPrimaryDesktopCoords(::i32_rectangle rectangle) override;
      void getDisplayNumberCoords(::i32_rectangle rectangle, ::u8 dispNumber) override;
      ::i32_rectangle_array_base getDisplaysCoords() override;
      void getNormalizedRect(::i32_rectangle rectangle) override;
      void getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle rectangle) override;
      ::operating_system::window getWindowHandleByName(const ::scoped_string &windowName) override;
      void getApplicationRegion(::u32 procId, Region & region) override;
      bool isApplicationInFocus(::u32 procId) override;

      void setKeyboardEvent(::u32 keySym, bool down) override;
      void setMouseEvent(::u16 x, ::u16 y, ::u8 buttonMask) override;
      void setNewClipText(const ::scoped_string &newClipboard) override;

   //protected:
      // Calling when at least one update has been detected.
      void onUpdate() override;
      // Implementation of the UpdateRequestListener interface.
      void onUpdateRequest(const ::i32_rectangle &rectRequested, bool incremental) override;
      // Calling when a clipbard change detected.
      void onClipboardUpdate(const ::scoped_string &newClipboard) override;
      // Calling when a configuration has been reloaded.
      // Uses to update internal settings.
      void onConfigReload(::remoting_rfb_node::ServerConfig *serverConfig) override;
      virtual void applyNewConfiguration() = 0;

      // Returns true when a remote input allowed.
      bool isRemoteInputAllowed();
      // This is an auxiliary function which determines that
      virtual bool isRemoteInputTempBlocked() = 0;

      bool updateExternalFramebuffer(::innate_subsystem::Framebuffer *pframebuffer, const Region & region,
                                             const ::i32_rectangle &rectangleViewport) override;

      void sendUpdate();


   };


} // namespace remoting_rfb






