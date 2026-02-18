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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "WindowsUserInput.h"
#include "thread/DesktopSelector.h"
//#include "win_system/Environment.h"
#include "win_system/Keyboard.h"
#include "gui/WindowFinder.h"
#include "util/BrokenHandleException.h"

WindowsUserInput::WindowsUserInput(ClipboardListener *clipboardListener,
                                   bool ctrlAltDelEnabled,
                                   LogWriter *log)
: m_prevKeyFlag(0),
  m_inputInjector(ctrlAltDelEnabled, log),
  m_log(log)
{
  m_clipboard = new WindowsClipboard(clipboardListener, m_log);
}

WindowsUserInput::~WindowsUserInput(void)
{
  delete m_clipboard;
}

// FIXME: refactor this horror.
void WindowsUserInput::setMouseEvent(const Point newPos, unsigned char keyFlag)
{
	m_log->debug("setMouseEvent ({},{}):{}", newPos.x, newPos.x, keyFlag);
  if (GetSystemMetrics(SM_SWAPBUTTON))
  {
    // read values of first and third bytes..
    unsigned char left = keyFlag & 1;
    unsigned char right = keyFlag & 4;
    // set them to zero..
    keyFlag &= 0xFA;
    // and set swapped values
    keyFlag |= (right & 4)  >> 2;
    keyFlag |= left << 2;
  }

  DWORD dwFlags = 0;
  dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

  bool prevState;
  bool currState;
  // Check the left button on change state
  prevState = (m_prevKeyFlag & 1) != 0;
  currState = (keyFlag       & 1) != 0;
  if (currState != prevState) {
    if (currState) {
      dwFlags |= MOUSEEVENTF_LEFTDOWN;
    } else {
      dwFlags |= MOUSEEVENTF_LEFTUP;
    }
  }
  // Check the middle button on change state
  prevState = (m_prevKeyFlag & 2) != 0;
  currState = (keyFlag       & 2) != 0;
  if (currState != prevState) {
    if (currState) {
      dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
    } else {
      dwFlags |= MOUSEEVENTF_MIDDLEUP;
    }
  }
  // Check the right button on change state
  prevState = (m_prevKeyFlag & 4) != 0;
  currState = (keyFlag       & 4) != 0;
  if (currState != prevState) {
    if (currState) {
      dwFlags |= MOUSEEVENTF_RIGHTDOWN;
    } else {
      dwFlags |= MOUSEEVENTF_RIGHTUP;
    }
  }

  // Check on a mouse wheel
  DWORD mouseWheelValue = 0;
  bool prevWheelUp = (m_prevKeyFlag & 8) != 0;
  bool currWheelUp = (keyFlag       & 8) != 0;
  bool prevWheelDown = (m_prevKeyFlag & 16) != 0;
  bool currWheelDown = (keyFlag       & 16) != 0;

  if (currWheelUp && !prevWheelUp) {
    dwFlags |= MOUSEEVENTF_WHEEL;
    mouseWheelValue = 120;
  } else if (currWheelDown && !prevWheelDown) {
    dwFlags |= MOUSEEVENTF_WHEEL;
    mouseWheelValue = -120;
  }

  m_prevKeyFlag = keyFlag;

  // Normilize pointer position
  unsigned short desktopWidth = GetSystemMetrics(SM_CXSCREEN);
  unsigned short desktopHeight = GetSystemMetrics(SM_CYSCREEN);
  int fbOffsetX = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int fbOffsetY = GetSystemMetrics(SM_YVIRTUALSCREEN);
  int x = (int)((newPos.x + fbOffsetX) * 65535 / (desktopWidth - 1));
  int y = (int)((newPos.y + fbOffsetY)* 65535 / (desktopHeight - 1));

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

void WindowsUserInput::setNewClipboard(const ::scoped_string & newClipboard)
{
  // FIXME: use ::string instead TCHAR * in writeToClipBoard arg
  m_clipboard->writeToClipBoard(newClipboard->getString());
}

void WindowsUserInput::setKeyboardEvent(unsigned int keySym, bool down)
{
  try {
    m_log->information("Received the %#4.4x keysym, down = {}", keySym, (int)down);
    // Generate single key event.
    BYTE vkCode;
    WCHAR ch;
    bool release = !down;
    bool extended;

    if (m_keyMap.keySymToVirtualCode(keySym, &vkCode, &extended)) {
      m_inputInjector.injectKeyEvent(vkCode, release, extended);
    } else if (m_keyMap.keySymToUnicodeChar(keySym, &ch)) {
      m_inputInjector.injectCharEvent(ch, release);
    } else {
      ::string scopedstrMessage;
      scopedstrMessage.formatf("Unknown {} keysym", keySym);
      throw ::remoting::Exception(scopedstrMessage);
    }
  } catch (::remoting::Exception &someEx) {
    m_log->error("::remoting::Exception while processing key event: {}", someEx.get_message());
  }
}

void WindowsUserInput::getCurrentUserInfo(::string & desktopName,
                                          ::string & userName)
{
  if (!DesktopSelector::getCurrentDesktopName(desktopName) &&
	  !Environment::getCurrentUserName(userName, m_log)) {
        ::string errMess;
        Environment::getErrStr("Can't get current user info", &errMess);
		throw ::remoting::Exception(errMess);
  }
}

void WindowsUserInput::getPrimaryDisplayCoords(::int_rectangle *prect)
{
  prect->left = 0;
  prect->top = 0;
  prect->right = GetSystemMetrics(SM_CXSCREEN);
  prect->bottom = GetSystemMetrics(SM_CYSCREEN);
  prect->move(-GetSystemMetrics(SM_XVIRTUALSCREEN),
             -GetSystemMetrics(SM_YVIRTUALSCREEN));
}

void WindowsUserInput::getDisplayNumberCoords(::int_rectangle *rect,
                                              unsigned char dispNumber)
{
  m_winDisplays.getDisplayCoordinates(dispNumber, rect);
}

::array_base<::int_rectangle> WindowsUserInput::getDisplaysCoords()
{
  return m_winDisplays.getDisplaysCoords();
}

void WindowsUserInput::getNormalizedRect(::int_rectangle *rect)
{
  toFbCoordinates(rect);
}

void WindowsUserInput::toFbCoordinates(::int_rectangle *rect)
{
  rect.move(-GetSystemMetrics(SM_XVIRTUALSCREEN),
             -GetSystemMetrics(SM_YVIRTUALSCREEN));
}

void WindowsUserInput::getWindowCoords(HWND hwnd, ::int_rectangle *rect)
{
  rect.clear();
  RECT winRect;
  if (GetWindowRect(hwnd, &winRect)) {
    rect.fromWindowsRect(&winRect);
    rect.move(-GetSystemMetrics(SM_XVIRTUALSCREEN),
               -GetSystemMetrics(SM_YVIRTUALSCREEN));
  } else {
    ::string errMess;
    Environment::getErrStr("Can't get window coordinates", &errMess);
    throw BrokenHandleException(errMess);
  }
}

HWND WindowsUserInput::getWindowHandleByName(const ::scoped_string & windowName)
{
  return WindowFinder::findFirstWindowByName(*windowName);
}

void WindowsUserInput::getApplicationRegion(unsigned int procId, Region *region)
{
  region->clear();
  HWND hForegr = GetWindow(GetForegroundWindow(), GW_HWNDLAST);

  RECT winRect;
  ::int_rectangle rect;
  while (hForegr != NULL) {
    GetWindowRect(hForegr, &winRect);

    DWORD style = GetWindowLong(hForegr, GWL_STYLE);
    DWORD procForegr;
    GetWindowThreadProcessId(hForegr, &procForegr);
    if (style & WS_VISIBLE) {
      rect.fromWindowsRect(&winRect);
      if (procForegr == procId) {
        region->addRect(&rect);
      } else {
        region->subtract(&Region(&rect));
      }
    }
    hForegr = GetWindow(hForegr, GW_HWNDPREV);
  }
  region->translate(-GetSystemMetrics(SM_XVIRTUALSCREEN),
                    -GetSystemMetrics(SM_YVIRTUALSCREEN));
}

bool WindowsUserInput::isApplicationInFocus(unsigned int procId)
{
  HWND hKeyboardInputWindow = GetForegroundWindow();
  if (hKeyboardInputWindow == NULL) {
    return false;
  }

  DWORD procForeground;
  GetWindowThreadProcessId(hKeyboardInputWindow, &procForeground);

  return (procForeground == (DWORD)procId);
}
