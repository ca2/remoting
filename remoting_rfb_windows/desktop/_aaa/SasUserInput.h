// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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


#include "UserInput.h"
#include "remoting/remoting_rfb/desktop_ipc/UserInputClient.h"


namespace remoting_rfb_windows
{

   // This class  delegates all inputs but the "ctrl+alt+del" under Vista or later
   // it process by himself.
   class CLASS_DECL_REMOTING_RFB SasUserInput : public UserInput
   {
   public:
      //SasUserInput(UserInputClient *client, ::subsystem::LogWriter * plogwriter);
      SasUserInput();
      ~SasUserInput() override;

      virtual void initialize_sas_user_input(UserInputClient *client, ::subsystem::LogWriter * plogwriter);

      virtual void sendInit(BlockingGate *pblockinggate);
      virtual void setNewClipboard(const ::scoped_string &newClipboard);
      virtual void setMouseEvent(const ::i32_point pointNewPosition, ::u8 keyFlag);
      virtual void setKeyboardEvent(::u32 keySym, bool down);
      virtual void getCurrentUserInfo(::string &desktopName, ::string &userName);
      virtual void getPrimaryDisplayCoords(::i32_rectangle rectangle);
      virtual void getDisplayNumberCoords(::i32_rectangle rectangle, ::u8 dispNumber);
      virtual ::int_rectangle_array_base getDisplaysCoords();
      virtual void getNormalizedRect(::i32_rectangle rectangle);
      virtual void getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle rectangle);
      virtual ::operating_system::window getWindowHandleByName(const ::scoped_string &windowName);
      virtual void getApplicationRegion(::u32 procId, Region & region);
      virtual bool isApplicationInFocus(::u32 procId);

   private:
      UserInputClient *m_client;

      bool m_ctrlPressed;
      bool m_altPressed;
      bool m_underVista;
      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting_rfb_windows







