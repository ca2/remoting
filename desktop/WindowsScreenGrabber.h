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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "remoting/remoting_common/util/CommonHeader.h"
#ifndef CAPTUREBLT
#define CAPTUREBLT          (DWORD)0x40000000 /* Include layered windows */


#include "ScreenGrabber.h"

#include "remoting/remoting_common/server_config/Configurator.h"
#include "remoting/remoting_common/win_system/WindowsEvent.h"
#include "remoting/remoting_common/thread/GuiThread.h"
#include "remoting/remoting_common/win_system/Screen.h"

//
// An abstract interface for screen grabbing.
//

/*
  //
  // Usage example:
  //

  ScreenGrabber *frameBuffer;

  // Initialisation
  frameBuffer = new WindowsScreenGrabber;

  ::int_rectangle grabRect, workRect;
  workRect.setRect(100, 100, 500, 500);
  grabRect.setRect(20, 20, 120, 120); // Relative to the workRect
  frameBuffer->setWorkRect(&workRect);

  // One-time grabbing
  while (!frameBuffer->grab(&grabRect)) {
    if (frameBuffer->getPropertiesChanged()) { // Check desktop properties
      if (!frameBuffer->applyNewProperties()) {
        MessageBox(NULL, "Cannot apply new screen properties"), _T("Error", MB_ICONHAND);
        return 1;
      }
    } else {
      MessageBox(NULL, "Cannot grab screen"), _T("Error", MB_ICONHAND);
      return 1;
    }
  }
*/

class WindowsScreenGrabber :
  public ScreenGrabber, public GuiThread
{
public:
  WindowsScreenGrabber(void);
  virtual ~WindowsScreenGrabber(void);

  virtual bool grab(const ::int_rectangle &  rect = 0);

  inline virtual bool getPropertiesChanged();
  inline virtual bool getPixelFormatChanged();
  inline virtual bool getScreenSizeChanged();

  virtual bool applyNewFullScreenRect();
  virtual bool applyNewPixelFormat();
  virtual bool applyNewProperties();

protected:
  virtual void execute();
  virtual void onTerminate();

  virtual bool openDIBSection();
  virtual bool closeDIBSection();
  virtual bool grabByDIBSection(const ::int_rectangle &  rect);

  // Windows specific variebles
  HDC m_destDC, m_screenDC;
  HBITMAP m_hbmOld, m_hbmDIB;

  WindowsEvent m_hasStartedSignal;
  WindowsEvent m_threadStopper;

private:
  ::int_size m_dibSectionDim;
  ServerConfig *m_serverConfig;

  Screen m_screen;
};

//// __WINDOWSSCREENGRABBER_H__
