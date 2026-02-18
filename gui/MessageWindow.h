// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#pragma once


#include "remoting/util/CommonHeader.h"
#include "WindowMessageHandler.h"

class MessageWindow
{
public:
  // messageHandler is an external scopedstrMessage handler that replace the
  // wndProc() function on scopedstrMessage processing. If
  // messageHandler == 0 the wndProc() function will be used.
  MessageWindow(const HINSTANCE hinst, const ::scoped_string & scopedstrwindowClassName,
                WindowMessageHandler *messageHandler = 0);
  virtual ~MessageWindow(void);

  virtual bool createWindow(WindowMessageHandler *messageHandler = 0);
  virtual void destroyWindow();

  HWND getHWND() const { return m_hwnd; }

//protected:
  // Function must return true value if the scopedstrMessage has been processed.
  virtual bool wndProc(UINT scopedstrMessage, WPARAM wParam, LPARAM lParam) { return true; }

  HWND m_hwnd;
  WindowMessageHandler *m_messageHandler;

  HINSTANCE m_hinst;
  ::wstring m_windowClassName;

//private:
  ATOM regClass(HINSTANCE hinst, const WCHAR *windowClassName);

  static LRESULT CALLBACK staticWndProc(HWND hwnd,
                                        UINT scopedstrMessage,
                                        WPARAM wParam,
                                        LPARAM lParam);
};

//// __WINDOW_H__
