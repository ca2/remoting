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


#include "UserInput.h"
//#include "WindowsClipboard.h"
#include "subsystem/platform/Keymap.h"
#include "subsystem/node/InputInjector.h"
#include "subsystem/node/Displays.h"
//#include "log_writer/LogWriter.h"


namespace remoting
{

   class CLASS_DECL_REMOTING WindowsUserInput : public UserInput
   {
   public:
      WindowsUserInput(::subsystem::ClipboardListener *clipboardListener, bool ctrlAltDelEnabled,
                       ::subsystem::LogWriter *log);
      virtual ~WindowsUserInput(void);

      virtual void setNewClipboard(const ::scoped_string &newClipboard);
      virtual void setMouseEvent(const ::int_point newPos, unsigned char keyFlag);
      virtual void setKeyboardEvent(unsigned int keySym, bool down);

      virtual void getCurrentUserInfo(::string &desktopName, ::string &userName);
      virtual void getDisplayNumberCoords(::int_rectangle *rect, unsigned char dispNumber);
      virtual ::array_base<::int_rectangle> WindowsUserInput::getDisplaysCoords();
      virtual void getNormalizedRect(::int_rectangle *rect);
      virtual void getPrimaryDisplayCoords(::int_rectangle *rect);
      virtual void getWindowCoords(HWND hwnd, ::int_rectangle *rect);
      virtual HWND getWindowHandleByName(const ::scoped_string &windowName);
      virtual void getApplicationRegion(unsigned int procId, ::remoting::Region *region);
      virtual bool isApplicationInFocus(unsigned int procId);

      virtual void initKeyFlag(unsigned char initValue) { m_prevKeyFlag = initValue; }

   protected:
      void toFbCoordinates(::int_rectangle *rect);

      ::pointer<::subsystem::Clipboard2> m_pclipboard;
      // WindowsDisplays m_winDisplays;
      ::subsystem::Displays m_winDisplays;

      ::subsystem::Keymap m_keyMap;
      ::subsystem::InputInjector m_inputInjector;

      unsigned char m_prevKeyFlag;

      ::subsystem::LogWriter *m_plogwriter;
   };

   //// __WINDOWSUSERINPUT_H__

} // namespace remoting






