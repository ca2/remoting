// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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


#include "subsystem/_common_header.h"
#include "remoting/remoting/desktop/UserInput.h"
#include "subsystem/node/ClipboardListener.h"
//#include "subsystem/platform/inttypes.h"
#include "DesktopServerProto.h"
#include "DesktopSrvDispatcher.h"

namespace remoting
{

   class CLASS_DECL_REMOTING  UserInputClient :
   virtual public UserInput,
   virtual public DesktopServerProto,
   virtual public ClientListener
   {
   public:
      //UserInputClient(BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher, ::subsystem::ClipboardListener *pclipboardlistener);
      UserInputClient();
      ~UserInputClient() override;


      virtual void initialize_user_input_client(::remoting_node::Configurator * pconfigurator, BlockingGate *pblockinggate, DesktopSrvDispatcher * pdispatcher, ::subsystem::ClipboardListener *pclipboardlistener);

      virtual void sendInit(BlockingGate *pblockinggate)  override;
      virtual void setNewClipboard(const ::scoped_string &newClipboard)  override;
      virtual void setMouseEvent(const ::i32_point pointNewPosition, unsigned char keyFlag)  override;
      virtual void setKeyboardEvent(::u32 keySym, bool down)  override;
      virtual void getCurrentUserInfo(::string &desktopName, ::string &userName)  override;
      virtual void getPrimaryDisplayCoords(::i32_rectangle & rectangle)  override;
      virtual void getDisplayNumberCoords(::i32_rectangle & rectangle, unsigned char dispNumber)  override;
      virtual ::int_rectangle_array_base getDisplaysCoords()  override;
      virtual void getNormalizedRect(::i32_rectangle & rectangle)  override;
      virtual void getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle)  override;
      virtual ::operating_system::window getWindowHandleByName(const ::scoped_string &windowName)  override;
      virtual void getApplicationRegion(const ::process_identifier & processidentifier, Region & region)  override;
      virtual bool isApplicationInFocus(const ::process_identifier & processidentifier)  override;

      // To catch a new clipboard
      virtual void onRequest(unsigned char reqCode, BlockingGate *pblockinggate)  override;

   ///protected:


      unsigned char m_sendMouseFlags;
      ::pointer < ::subsystem::ClipboardListener > m_pclipboardlistener;


   };


} // namespace remoting





