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
#include "remoting/remoting_rtc/desktop_ipc/UserInputClient.h"


namespace remoting
{

   // This class  delegates all inputs but the "ctrl+alt+del" under Vista or later
   // it process by himself.
   class CLASS_DECL_REMOTING SasUserInput : public UserInput
   {
   public:
      //SasUserInput(UserInputClient *client, ::subsystem::LogWriter * plogwriter);
      SasUserInput();
      ~SasUserInput() override;

      virtual void initialize_sas_user_input(UserInputClient *client, ::subsystem::LogWriter * plogwriter);

      void sendInit(BlockingGate *pblockinggate) override;
      void setNewClipboard(const ::scoped_string &newClipboard) override;
      void setMouseEvent(const ::i32_point pointNewPosition, unsigned char keyFlag) override;
      void setKeyboardEvent(::u32 keySym, bool down) override;
      void getCurrentUserInfo(::string &desktopName, ::string &userName) override;
      void getPrimaryDisplayCoords(::i32_rectangle & rectangle) override;
      void getDisplayNumberCoords(::i32_rectangle & rectangle, unsigned char dispNumber) override;
      ::int_rectangle_array_base getDisplaysCoords() override;
      void getNormalizedRect(::i32_rectangle & rectangle) override;
      void getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle) override;
      ::operating_system::window getWindowHandleByName(const ::scoped_string &windowName) override;
      void getApplicationRegion(const ::process_identifier & processidentifier, Region & region) override;
      bool isApplicationInFocus(const ::process_identifier & processidentifier) override;

   private:
      UserInputClient *m_client;

      bool m_ctrlPressed;
      bool m_altPressed;
      bool m_underVista;
      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting







