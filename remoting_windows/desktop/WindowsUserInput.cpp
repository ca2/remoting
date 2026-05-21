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
#include "framework.h"
#include "acme/constant/user_key.h"
#include "remoting/remoting_windows/desktop/WindowsUserInput.h"
#include "subsystem/thread/DesktopSelector.h"
#include "subsystem/node/OperatingSystem.h"
#include "subsystem/node/Keyboard.h"
#include "innate_subsystem/gui/WindowFinder.h"
#include "subsystem/node/Clipboard2.h"
#include "subsystem/platform/BrokenHandleException.h"

namespace remoting_windows
{


   // WindowsUserInput::WindowsUserInput(ClipboardListener *pclipboardlistener, bool ctrlAltDelEnabled,
   //                                    ::subsystem::LogWriter * plogwriter) :
   //     m_prevKeyFlag(0), m_inputInjector(ctrlAltDelEnabled, log), m_plogwriter(plogwriter)
   // {
   //    m_clipboard = new WindowsClipboard(pclipboardlistener, m_plogwriter);
   // }

   WindowsUserInput::WindowsUserInput() :
   m_prevKeyFlag(0)
   {
      //m_clipboard = new WindowsClipboard(pclipboardlistener, m_plogwriter);
   }



   WindowsUserInput::~WindowsUserInput(void) {  }

   void WindowsUserInput::initialize_user_input(::subsystem::ClipboardListener *pclipboardlistener, bool ctrlAltDelEnabled,
                 ::subsystem::LogWriter * plogwriter)
   {
    m_prevKeyFlag = 0;
      m_inputInjector.initialize_input_injector(ctrlAltDelEnabled, plogwriter);
      m_plogwriter = plogwriter;
      constructø(m_pclipboard);
      m_pclipboard->initialize_clipboard2(pclipboardlistener, plogwriter);
   }


   // FIXME: refactor this horror.
   void WindowsUserInput::setMouseEvent(const ::i32_point pointNewPosition, unsigned char keyFlag)
   {
      m_plogwriter->debug("setMouseEvent ({},{}):{}", pointNewPosition.x, pointNewPosition.x, keyFlag);
      if (GetSystemMetrics(SM_SWAPBUTTON))
      {
         // read values of first and third bytes..
         unsigned char left = keyFlag & 1;
         unsigned char right = keyFlag & 4;
         // set them to zero..
         keyFlag &= 0xFA;
         // and set swapped values
         keyFlag |= (right & 4) >> 2;
         keyFlag |= left << 2;
      }

      DWORD dwFlags = 0;
      dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

      bool prevState;
      bool currState;
      // Check the left button on change state
      prevState = (m_prevKeyFlag & 1) != 0;
      currState = (keyFlag & 1) != 0;
      if (currState != prevState)
      {
         if (currState)
         {
            dwFlags |= MOUSEEVENTF_LEFTDOWN;
         }
         else
         {
            dwFlags |= MOUSEEVENTF_LEFTUP;
         }
      }
      // Check the middle button on change state
      prevState = (m_prevKeyFlag & 2) != 0;
      currState = (keyFlag & 2) != 0;
      if (currState != prevState)
      {
         if (currState)
         {
            dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
         }
         else
         {
            dwFlags |= MOUSEEVENTF_MIDDLEUP;
         }
      }
      // Check the right button on change state
      prevState = (m_prevKeyFlag & 4) != 0;
      currState = (keyFlag & 4) != 0;
      if (currState != prevState)
      {
         if (currState)
         {
            dwFlags |= MOUSEEVENTF_RIGHTDOWN;
         }
         else
         {
            dwFlags |= MOUSEEVENTF_RIGHTUP;
         }
      }

      // Check on a mouse wheel
      DWORD mouseWheelValue = 0;
      bool prevWheelUp = (m_prevKeyFlag & 8) != 0;
      bool currWheelUp = (keyFlag & 8) != 0;
      bool prevWheelDown = (m_prevKeyFlag & 16) != 0;
      bool currWheelDown = (keyFlag & 16) != 0;

      if (currWheelUp && !prevWheelUp)
      {
         dwFlags |= MOUSEEVENTF_WHEEL;
         mouseWheelValue = 120;
      }
      else if (currWheelDown && !prevWheelDown)
      {
         dwFlags |= MOUSEEVENTF_WHEEL;
         mouseWheelValue = -120;
      }

      m_prevKeyFlag = keyFlag;

      // Normilize pointer pointPosition
      unsigned short desktopWidth = GetSystemMetrics(SM_CXSCREEN);
      unsigned short desktopHeight = GetSystemMetrics(SM_CYSCREEN);
      int fbOffsetX = GetSystemMetrics(SM_XVIRTUALSCREEN);
      int fbOffsetY = GetSystemMetrics(SM_YVIRTUALSCREEN);
      int x = (int)((pointNewPosition.x + fbOffsetX) * 65535 / (desktopWidth - 1));
      int y = (int)((pointNewPosition.y + fbOffsetY) * 65535 / (desktopHeight - 1));

      INPUT input;
      memset(&input, 0, sizeof(INPUT));
      input.type = INPUT_MOUSE;
      input.mi.dwFlags = dwFlags;
      input.mi.dx = x;
      input.mi.dy = y;
      input.mi.mouseData = mouseWheelValue;
      SendInput(1, &input, sizeof(INPUT));
      DWORD error = GetLastError();
   }

   void WindowsUserInput::setNewClipboard(const ::scoped_string &newClipboard)
   {
      // FIXME: use ::string instead TCHAR * in writeToClipBoard arg
      m_pclipboard->writeToClipBoard(newClipboard);
   }

   void WindowsUserInput::setKeyboardEvent(::u32 keySym, bool down)
   {
      try
      {
         m_plogwriter->information("Received the %#4.4x keysym, down = {}", keySym, (int)down);
         // Generate single key event.
         //unsigned char vkCode;
         ::user::enum_key euserkey = ::user::e_key_none;
         int ch;
         bool release = !down;
         bool extended;

         if (m_keyMap.keySymToVirtualCode(keySym, &euserkey, &extended))
         {
            m_inputInjector.injectKeyEvent(euserkey, release, extended);
         }
         else if (m_keyMap.keySymToUnicodeChar(keySym, &ch))
         {
            m_inputInjector.injectCharEvent(ch, release);
         }
         else
         {
            ::string scopedstrMessage;
            scopedstrMessage.format("Unknown {} keysym", keySym);
            throw ::subsystem::Exception(scopedstrMessage);
         }
      }
      catch (::subsystem::Exception &someEx)
      {
         m_plogwriter->error("::subsystem::Exception while processing key event: {}", someEx.get_message());
      }
   }

   void WindowsUserInput::getCurrentUserInfo(::string &desktopName, ::string &userName)
   {
      if (!MainSubsystem().DesktopSelector().getCurrentDesktopName(desktopName) &&
          !MainSubsystem().OperatingSystem().getCurrentUserName(userName, m_plogwriter))
      {
         ::string errMess;
         errMess  = MainSubsystem().OperatingSystem().getErrStr("Can't get current user info");
         throw ::subsystem::Exception(errMess);
      }
   }

   void WindowsUserInput::getPrimaryDisplayCoords(::i32_rectangle & rectangle)
   {
      rectangle.left = 0;
      rectangle.top = 0;
      rectangle.right = GetSystemMetrics(SM_CXSCREEN);
      rectangle.bottom = GetSystemMetrics(SM_CYSCREEN);
      rectangle.offset(-GetSystemMetrics(SM_XVIRTUALSCREEN), -GetSystemMetrics(SM_YVIRTUALSCREEN));
   }

   void WindowsUserInput::getDisplayNumberCoords(::i32_rectangle & rectangle, unsigned char dispNumber)
   {
      m_winDisplays.getDisplayCoordinates(dispNumber, rectangle);
   }

   ::int_rectangle_array_base WindowsUserInput::getDisplaysCoords() { return m_winDisplays.getDisplaysCoords(); }

   void WindowsUserInput::getNormalizedRect(::i32_rectangle & rectangle) { toFbCoordinates(rectangle); }

   void WindowsUserInput::toFbCoordinates(::i32_rectangle & rectangle)
   {
      rectangle.offset(-GetSystemMetrics(SM_XVIRTUALSCREEN), -GetSystemMetrics(SM_YVIRTUALSCREEN));
   }

   void WindowsUserInput::getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle)
   {
      rectangle.clear();
      //RECT rectangle;
      if (::windows::get_window_rect(operatingsystemwindow, rectangle))
      {
//         rectangle.fromWindowsRect(&rectangle);
         rectangle.offset(-GetSystemMetrics(SM_XVIRTUALSCREEN), -GetSystemMetrics(SM_YVIRTUALSCREEN));
      }
      else
      {
         ::string errMess;
         errMess = MainSubsystem().OperatingSystem().getErrStr("Can't get window coordinates");
         throw ::subsystem::BrokenHandleException(errMess);
      }
   }

   operating_system::window WindowsUserInput::getWindowHandleByName(const ::scoped_string &windowName)
   {
      return ::windows::findFirstWindowByName(windowName);
   }

   void WindowsUserInput::getApplicationRegion(const ::process_identifier & processidentifier, ::remoting::Region & region)
   {
      region.clear();
      auto operatingsystemwindowForeground = ::windows::get_window(::windows::get_foreground_window(), WIN32_GW_HWNDLAST);

      //RECT rectangle;
      ::i32_rectangle rectangle;
      while (operatingsystemwindowForeground.is_set())
      {
         ::windows::get_window_rect(operatingsystemwindowForeground, rectangle);

         auto style = ::windows::get_window_long(operatingsystemwindowForeground, GWL_STYLE);
         auto processidentifierForegrund = ::windows::get_window_process_id(operatingsystemwindowForeground);
         //GetWindowThreadProcessId(hForegr, &procForegr);
         if (style & WS_VISIBLE)
         {
            if (processidentifierForegrund == processidentifier)
            {
               region.addRect(rectangle);
            }
            else
            {
               region.subtract(::remoting::Region(rectangle));
            }
         }
         operatingsystemwindowForeground = ::windows::get_window(operatingsystemwindowForeground, WIN32_GW_HWNDPREV);
      }
      region.translate(-GetSystemMetrics(SM_XVIRTUALSCREEN), -GetSystemMetrics(SM_YVIRTUALSCREEN));
   }

   bool WindowsUserInput::isApplicationInFocus(const ::process_identifier & processidentifier)
   {
      auto operatingsystemKeyboardInputWindow = ::windows::get_foreground_window();
      if (operatingsystemKeyboardInputWindow.is_null())
      {
         return false;
      }

      auto processidentifiderForeground = ::windows::get_window_process_id(operatingsystemKeyboardInputWindow);

      return processidentifiderForeground == processidentifier;
   }


} // namespace remoting_windows
