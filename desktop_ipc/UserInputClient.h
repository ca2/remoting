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


#include "remoting/remoting_common/util/CommonHeader.h"
#include "desktop/UserInput.h"
#include "desktop/ClipboardListener.h"
#include "remoting/remoting_common/util/inttypes.h"
#include "DesktopServerProto.h"
#include "DesktopSrvDispatcher.h"

class UserInputClient : public UserInput, public DesktopServerProto,
                        public ClientListener
{
public:
  UserInputClient(BlockingGate *forwGate, DesktopSrvDispatcher *dispatcher,
                  ClipboardListener *clipboardListener);
  virtual ~UserInputClient();

  virtual void sendInit(BlockingGate *gate);
  virtual void setNewClipboard(const ::scoped_string & newClipboard);
  virtual void setMouseEvent(const Point newPos, unsigned char keyFlag);
  virtual void setKeyboardEvent(unsigned int keySym, bool down);
  virtual void getCurrentUserInfo(::string & desktopName,
                                  ::string & userName);
  virtual void getPrimaryDisplayCoords(::int_rectangle *rect);
  virtual void getDisplayNumberCoords(::int_rectangle *rect,
                                      unsigned char dispNumber);
  virtual ::array_base<::int_rectangle> getDisplaysCoords();
  virtual void getNormalizedRect(::int_rectangle *rect);
  virtual void getWindowCoords(HWND hwnd, ::int_rectangle *rect);
  virtual HWND getWindowHandleByName(const ::scoped_string & windowName);
  virtual void getApplicationRegion(unsigned int procId, Region *region);
  virtual bool isApplicationInFocus(unsigned int procId);

  // To catch a new clipboard
  virtual void onRequest(unsigned char reqCode, BlockingGate *backGate);

protected:
  unsigned char m_sendMouseFlags;
  ClipboardListener *m_clipboardListener;
};

//// __USERINPUTCLIENT_H__
