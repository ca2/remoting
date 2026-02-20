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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


//////#include "remoting/remoting_common/util/::string.h"

#include "remoting/remoting_common/region/Region.h"
#include "remoting/remoting_common/rfb/PixelFormat.h"
#include "remoting/remoting_common/rfb/FrameBuffer.h"
#include "remoting/remoting_common/fb_update_sender/UpdateRequestListener.h"
//#include <vector>

// This class is a public interface to a desktop.
class Desktop : public UpdateRequestListener
{
public:
  virtual ~Desktop() {}

  // Puts a current desktop name from working session to the
  // desktopName argument and an user name to userMame.
  virtual void getCurrentUserInfo(::string & desktopName,
                                  ::string & userName) = 0;
  // Puts the current frame buffer dimension and pixel format to
  // the dim and pf function arguments.
  virtual void getFrameBufferProperties(::int_size *dim, PixelFormat *pf) = 0;

  virtual void getPrimaryDesktopCoords(::int_rectangle *rect) = 0;
  // Returns a rect that is normilized from "virtual desktop" to frame buffer coordinates.
  virtual void getNormalizedRect(::int_rectangle *rect) = 0;
  virtual void getDisplayNumberCoords(::int_rectangle *rect,
                                      unsigned char dispNumber) = 0;
  virtual ::array_base<::int_rectangle> getDisplaysCoords() = 0;
  virtual void getWindowCoords(HWND hwnd, ::int_rectangle *rect) = 0;
  virtual HWND getWindowHandleByName(const ::scoped_string & windowName) = 0;

  virtual void getApplicationRegion(unsigned int procId, Region *region) = 0;
  virtual bool isApplicationInFocus(unsigned int procId) = 0;

  virtual void setKeyboardEvent(unsigned int keySym, bool down) = 0;
  virtual void setMouseEvent(unsigned short x, unsigned short y, unsigned char buttonMask) = 0;
  virtual void setNewClipText(const ::scoped_string & newClipboard) = 0;

  // Updates external frame buffer pixels only for the region from view port
  // located at the place in a central frame buffer.
  // If view port is out of central frame buffer bounds the function will return false.
  virtual bool updateExternalFrameBuffer(FrameBuffer *fb, const Region *region,
                                         const ::int_rectangle &  viewPort) = 0;
};

//// __DESKTOP_H__
