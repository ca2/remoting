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

#include "remoting/remoting_macos/_common_header.h"
#include "remoting/remoting_rfb/desktop/UserInput.h"
#include "subsystem/platform/Keymap.h"
#include "subsystem/node/InputInjector.h"


namespace remoting_macos
{

   class CLASS_DECL_REMOTING_MACOS MacosUserInput :
      virtual public ::remoting_rfb::UserInput
   {
   public:

      ::pointer<::subsystem::Clipboard2> m_pclipboard;
      ::subsystem::Keymap m_keyMap;
      ::subsystem::InputInjector m_inputInjector;

      ::u8 m_prevKeyFlag;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      MacosUserInput();
      ~MacosUserInput() override;

      void initialize_user_input(::subsystem::ClipboardListener *pclipboardlistener, bool ctrlAltDelEnabled,
                 ::subsystem::LogWriter * plogwriter) override;

      void setNewClipboard(const ::scoped_string &newClipboard) override;
      void setMouseEvent(const ::i32_point pointNewPosition, ::u8 keyFlag) override;
      void setKeyboardEvent(::u32 keySym, bool down) override;

      void getCurrentUserInfo(::string &desktopName, ::string &userName) override;
      void getDisplayNumberCoords(::i32_rectangle & rectangle, ::u8 dispNumber) override;
      ::i32_rectangle_array_base getDisplaysCoords() override;
      void getNormalizedRect(::i32_rectangle & rectangle) override;
      void getPrimaryDisplayCoords(::i32_rectangle & rectangle) override;
      void getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle) override;
      ::operating_system::window getWindowHandleByName(const ::scoped_string &windowName) override;
      void getApplicationRegion(const ::process_identifier & processidentifier, ::remoting_rfb::Region & region) override;
      bool isApplicationInFocus(const ::process_identifier & processidentifier) override;

      void initKeyFlag(::u8 initValue) override { m_prevKeyFlag = initValue; }

      void toFbCoordinates(::i32_rectangle & rectangle);

   };

} // namespace remoting_macos






