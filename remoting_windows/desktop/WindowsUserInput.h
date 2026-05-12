// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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

#include "remoting/remoting_macos/_common_header.h"
#include "remoting/remoting/desktop/UserInput.h"
//#include "remoting/remoting_macos/desktop/WindowsClipboard.h"
#include "subsystem/platform/Keymap.h"
#include "subsystem/node/InputInjector.h"
#include "subsystem/node/Displays.h"
//#include "log_writer/LogWriter.h"


namespace remoting_macos
{

   class CLASS_DECL_REMOTING_MACOS WindowsUserInput :
   virtual public ::remoting::UserInput
   {
   public:

      ::pointer<::subsystem::Clipboard2> m_pclipboard;
      // WindowsDisplays m_winDisplays;
      ::subsystem::Displays m_winDisplays;

      ::subsystem::Keymap m_keyMap;
      ::subsystem::InputInjector m_inputInjector;

      unsigned char m_prevKeyFlag;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;


      //WindowsUserInput(::subsystem::ClipboardListener *pclipboardlistener, bool ctrlAltDelEnabled,
        //               ::subsystem::LogWriter * plogwriter);
      WindowsUserInput();
      ~WindowsUserInput() override;

      void initialize_user_input(::subsystem::ClipboardListener *pclipboardlistener, bool ctrlAltDelEnabled,
                 ::subsystem::LogWriter * plogwriter) override;

      virtual void setNewClipboard(const ::scoped_string &newClipboard);
      virtual void setMouseEvent(const ::i32_point pointNewPosition, unsigned char keyFlag);
      virtual void setKeyboardEvent(::u32 keySym, bool down);

      virtual void getCurrentUserInfo(::string &desktopName, ::string &userName);
      virtual void getDisplayNumberCoords(::i32_rectangle & rectangle, unsigned char dispNumber);
      virtual ::int_rectangle_array_base WindowsUserInput::getDisplaysCoords();
      virtual void getNormalizedRect(::i32_rectangle & rectangle);
      virtual void getPrimaryDisplayCoords(::i32_rectangle & rectangle);
      virtual void getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle);
      virtual ::operating_system::window getWindowHandleByName(const ::scoped_string &windowName);
      virtual void getApplicationRegion(const ::process_identifier & processidentifier, ::remoting::Region & region);
      virtual bool isApplicationInFocus(const ::process_identifier & processidentifier);

      void initKeyFlag(unsigned char initValue) override { m_prevKeyFlag = initValue; }

   //protected:
      void toFbCoordinates(::i32_rectangle & rectangle);

   };

   //// __WINDOWSUSERINPUT_H__

} // namespace remoting_macos






