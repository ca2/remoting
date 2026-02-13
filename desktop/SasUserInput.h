// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#ifndef __SASUSERINPUT_H__
#define __SASUSERINPUT_H__

#include "UserInput.h"
#include "desktop_ipc/UserInputClient.h"

// This class delegates all inputs but the "ctrl+alt+del" under Vista or later
// it process by himself.
class SasUserInput : public UserInput
{
public:
  SasUserInput(UserInputClient *client, LogWriter *log);
  virtual ~SasUserInput();

  virtual void sendInit(BlockingGate *gate);
  virtual void setNewClipboard(const ::string & newClipboard);
  virtual void setMouseEvent(const Point newPos, unsigned char keyFlag);
  virtual void setKeyboardEvent(unsigned int keySym, bool down);
  virtual void getCurrentUserInfo(::string & desktopName,
                                  ::string & userName);
  virtual void getPrimaryDisplayCoords(::int_rectangle *rect);
  virtual void getDisplayNumberCoords(::int_rectangle *rect,
                                      unsigned char dispNumber);
  virtual ::std::vector<::int_rectangle> getDisplaysCoords();
  virtual void getNormalizedRect(::int_rectangle *rect);
  virtual void getWindowCoords(HWND hwnd, ::int_rectangle *rect);
  virtual HWND getWindowHandleByName(const ::string & windowName);
  virtual void getApplicationRegion(unsigned int procId, Region *region);
  virtual bool isApplicationInFocus(unsigned int procId);

private:
  UserInputClient *m_client;

  bool m_ctrlPressed;
  bool m_altPressed;
  bool m_underVista;
  LogWriter *m_log;
};

#endif // __SASUSERINPUT_H__
