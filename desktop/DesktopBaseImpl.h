// Copyright (C) 2011,2012 GlavSoft LLC.
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


#include "Desktop.h"
#include "UpdateHandler.h"
#include "server_config_lib/ConfigReloadListener.h"
#include "UserInput.h"
// External listeners
#include "AbnormDeskTermListener.h"
#include "UpdateSendingListener.h"
#include "ClipboardListener.h"

// This class is a base class for different implemetations of desktops
class DesktopBaseImpl : public Desktop,
                   public UpdateListener,
                   public ClipboardListener,
                   public ConfigReloadListener
{
public:
  DesktopBaseImpl(ClipboardListener *extClipListener,
             UpdateSendingListener *extUpdSendingListener,
             AbnormDeskTermListener *extDeskTermListener,
             LogWriter *log);
  virtual ~DesktopBaseImpl();

  // Puts a current desktop name from working session to the
  // desktopName argument and an user name to userMame.
  virtual void getCurrentUserInfo(::string & desktopName,
                                  ::string & userName);
  // Puts the current frame buffer dimension and pixel format to
  // the dim and pf function arguments.
  virtual void getFrameBufferProperties(::int_size *dim, PixelFormat *pf);
  virtual void getPrimaryDesktopCoords(::int_rectangle *rect);
  virtual void getDisplayNumberCoords(::int_rectangle *rect,
                                      unsigned char dispNumber);
  virtual ::array_base<::int_rectangle> getDisplaysCoords();
  virtual void getNormalizedRect(::int_rectangle *rect);
  virtual void getWindowCoords(HWND hwnd, ::int_rectangle *rect);
  virtual HWND getWindowHandleByName(const ::scoped_string & windowName);
  virtual void getApplicationRegion(unsigned int procId, Region *region);
  virtual bool isApplicationInFocus(unsigned int procId);

  virtual void setKeyboardEvent(unsigned int keySym, bool down);
  virtual void setMouseEvent(unsigned short x, unsigned short y, unsigned char buttonMask);
  virtual void setNewClipText(const ::scoped_string & newClipboard);

protected:
  // Calling when at least one update has been detected.
  virtual void onUpdate();
  // Implementation of the UpdateRequestListener interface.
  virtual void onUpdateRequest(const ::int_rectangle &  rectRequested, bool incremental);
  // Calling when a clipbard change detected.
  virtual void onClipboardUpdate(const ::scoped_string & newClipboard);
  // Calling when a configuration has been reloaded.
  // Uses to update internal settings.
  virtual void onConfigReload(ServerConfig *serverConfig);
  virtual void applyNewConfiguration() = 0;

  // Returns true when a remote input allowed.
  bool isRemoteInputAllowed();
  // This is an auxiliary function which determines that
  virtual bool isRemoteInputTempBlocked() = 0;

  virtual bool updateExternalFrameBuffer(FrameBuffer *fb, const Region *region,
                                         const ::int_rectangle &  viewPort);

  void sendUpdate();

  Region m_fullReqRegion;
  LocalMutex m_reqRegMutex;

  UpdateHandler *m_updateHandler;

  // A derived class thread control.
  WindowsEvent m_newUpdateEvent;

  UserInput *m_userInput;

  // Clipboard
  ::string m_receivedClip;
  LocalMutex m_storedClipCritSec;

  // External listeners
  UpdateSendingListener *m_extUpdSendingListener;
  AbnormDeskTermListener *m_extDeskTermListener;
  ClipboardListener *m_extClipListener;

  LogWriter *m_log;
};

//// __DESKTOPBASEIMPL_H__
