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

#pragma once


#include "util/inttypes.h"
#include "rfb/FrameBuffer.h"
#include "region/Region.h"

#include "region/Point.h"
//#include "util/::earth::time.h"

#include "BlockingGate.h"

class DesktopServerProto
{
protected:
  DesktopServerProto(BlockingGate *forwGate);
  virtual ~DesktopServerProto();

  virtual void readPixelFormat(PixelFormat *pf,
                               BlockingGate *gate);
  virtual void sendPixelFormat(const PixelFormat & pf,
                               BlockingGate *gate);
  virtual ::int_size readDimension(BlockingGate *gate);
  virtual void sendDimension(const ::int_size & dim,
                             BlockingGate *gate);
  virtual Point readPoint(BlockingGate *gate);
  virtual void sendPoint(const Point *point,
                         BlockingGate *gate);
  virtual ::int_rectangle readRect(BlockingGate *gate);
  virtual void sendRect(const ::int_rectangle &  rect,
                        BlockingGate *gate);
  virtual void sendRegion(const Region *region,
                          BlockingGate *gate);
  virtual void readRegion(Region *region,
                          BlockingGate *gate);

  void sendFrameBuffer(const FrameBuffer *srcFb, const ::int_rectangle &  srcRect,
                       BlockingGate *gate);
  void readFrameBuffer(FrameBuffer *dstFb, const ::int_rectangle &  dstRect,
                       BlockingGate *gate);

  virtual void sendNewClipboard(const ::scoped_string & newClipboard,
                                BlockingGate *gate);
  virtual void readNewClipboard(::string & newClipboard,
                                BlockingGate *gate);
  virtual void sendNewPointerPos(const Point newPos, unsigned char keyFlag,
                                 BlockingGate *gate);
  virtual void readNewPointerPos(Point *newPos, unsigned char *keyFlag,
                                 BlockingGate *gate);
  virtual void sendKeyEvent(unsigned int keySym, bool down,
                            BlockingGate *gate);
  virtual void readKeyEvent(unsigned int *keySym, bool *down,
                            BlockingGate *gate);
  virtual void sendUserInfo(const ::scoped_string & desktopName,
                            const ::scoped_string & userName,
                            BlockingGate *gate);
  virtual void readUserInfo(::string & desktopName,
                            ::string & userName,
                            BlockingGate *gate);
  virtual void sendConfigSettings(BlockingGate *gate);
  virtual void readConfigSettings(BlockingGate *gate);

  // FIXME: Remove m_forwGate from this class.
  // Forward gate will send requests
  BlockingGate *m_forwGate;

  static const unsigned char EXTRACT_REQ = 0;
  static const unsigned char SCREEN_PROP_REQ = 1;
  static const unsigned char FRAME_BUFFER_INIT = 2;
  static const unsigned char SET_FULL_UPD_REQ_REGION = 3;
  static const unsigned char SET_EXCLUDING_REGION = 4;
  static const unsigned char UPDATE_DETECTED = 10;

  static const unsigned char CLIPBOARD_CHANGED = 30;
  static const unsigned char POINTER_POS_CHANGED = 31;
  static const unsigned char KEYBOARD_EVENT = 32;
  static const unsigned char USER_INPUT_INIT = 33;
  static const unsigned char USER_INFO_REQ = 34;
  static const unsigned char DESKTOP_COORDS_REQ = 35;
  static const unsigned char WINDOW_COORDS_REQ = 36;
  static const unsigned char WINDOW_HANDLE_REQ = 37;
  static const unsigned char DISPLAY_NUMBER_COORDS_REQ = 38;
  static const unsigned char APPLICATION_REGION_REQ = 39;
  static const unsigned char NORMALIZE_RECT_REQ = 40;
  static const unsigned char APPLICATION_CHECK_FOCUS = 41;
  static const unsigned char DISPLAYS_COORDS_REQ = 42;

  static const unsigned char CONFIG_RELOAD_REQ = 50;
  static const unsigned char SOFT_INPUT_ENABLING_REQ = 51;

private:
  void checkPixelFormat(const PixelFormat & pf);
  void checkRectangle(const ::int_rectangle &  rect);
  void checkDimension(const ::int_size & dim);
};

//// __DESKTOPSERVERPROTO_H__
