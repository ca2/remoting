// Copyright (C) 2012 GlavSoft LLC.
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

#ifndef _DESKTOP_WINDOW_H_
#define _DESKTOP_WINDOW_H_

#include "win_system/WinClipboard.h"
#include "gui/DibFrameBuffer.h"


#include "ScaleManager.h"
#include "client_config_lib/ConnectionConfig.h"
#include "gui/PaintWindow.h"
#include "gui/ScrollBar.h"
#include "gui/drawing/SolidBrush.h"
#include "gui/drawing/Graphics.h"
#include "rfb/RfbKeySym.h"
#include "viewer_core/RemoteViewerCore.h"
namespace remoting
{
   class toolbar;
   class style;
} // namespace remoting_impact

class DesktopWindow : public PaintWindow,
                      protected RfbKeySymListener
{
public:
   ::int_size m_sizeBuffer = {};
   HDC m_hdcBuffer = nullptr;
   HBITMAP m_hbitmapOld = nullptr;
   HBITMAP m_hbitmapBuffer = nullptr;
   ::pointer< ::remoting::toolbar > m_premotingtoolbar;
   ::pointer< ::remoting::style > m_premotingstyle;
   bool m_bShowCursor = false;
class ::time m_timeStartDesktopWindow;
  DesktopWindow(LogWriter *logWriter, ConnectionConfig *conConf);
  virtual ~DesktopWindow();
   virtual void _defer_update_double_buffering();
  void setClipboardData(const ::string & strText);
  void updateFramebuffer(const FrameBuffer * pframebuffer,
                         const ::int_rectangle &  dstRect);
  // this function must be called if size of image was changed
  // or the number of bits per pixel
  void setNewFramebuffer(const FrameBuffer * pframebuffer);

  // set scale of image, can -1 = Auto, in percent
  void setScale(int scale);
  // it returns the image width and height considering scale
  ::int_rectangle getViewerGeometry();
  // it returns the image width and height.
  ::int_rectangle getFrameBufferGeometry();
  // it return size of server frame buffer and pixelsize.
  void getServerGeometry(::int_rectangle *rect, int *pixelsize);

  void setConnected();
  void setViewerCore(RemoteViewerCore *viewerCore);

  // This function set state key "Ctrl", but not send data to server.
  void setCtrlState(const bool ctrlState);
  // This function set state key "Alt", but not send data to server.
  void setAltState(const bool altState);
  // This function return true, if key "Control" is pressed.
  bool getCtrlState() const;
  // This function return true, if key "Alt" is pressed.
  bool getAltState() const;

  // this function sends to remote viewer core
  // key what is pressed or unpressed
  void sendKey(WCHAR key, bool pressed);
  // this function sends to remote viewer core the combination
  // Ctrl + Alt + Del
  void sendCtrlAltDel();

  // Set function for m_winKeyIgnore.
  void setWinKeyIgnore(bool winKeyIgnore) { m_rfbKeySym->setWinKeyIgnore(winKeyIgnore); }

protected:
public:

  //
  // Overrides RfbKeySymListener::onRfbKeySymEvent().
  //
  void onRfbKeySymEvent(unsigned int rfbKeySym, bool down);

  //
  // Inherited from BaseWindow.
  //
  bool onMessage(UINT message, WPARAM wParam, LPARAM lParam);
  //void onPaint(DeviceContext *dc, PAINTSTRUCT *paintStruct);
   void onPaint() override;
  bool onCreate(LPCREATESTRUCT pcs);
  bool onDrawClipboard();
  bool onEraseBackground(HDC hdc);
  bool onDeadChar(WPARAM wParam, LPARAM lParam);
  bool onHScroll(WPARAM wParam, LPARAM lParam);
  bool onVScroll(WPARAM wParam, LPARAM lParam);
  bool onKey(WPARAM wParam, LPARAM lParam);
  bool onChar(WPARAM wParam, LPARAM lParam);
  bool onMouse(unsigned char mouseKeys, unsigned short wheelSpeed, POINT position);
  bool onSize(WPARAM wParam, LPARAM lParam);
  bool onDestroy();

  POINTS getViewerCoord(long xPos, long yPos);
  void calculateWndSize(bool isChanged);
  void applyScrollbarChanges(bool isChanged, bool isVert, bool isHorz, int wndWidth, int wndHeight);

  // This function check pointer to viewer core and send event.
  // If into viewer core throwing exception Exception, then it catching
  // in this function and logged.
  void sendKeyboardEvent(bool downFlag, unsigned int key);
  void sendPointerEvent(unsigned char buttonMask, const Point *position);
  void sendCutTextEvent(const ::string & cutText);

  LogWriter *m_logWriter;

  // This variable is true after call CoreEventsAdapter::onConnected().
  bool m_isConnected;

  // keyboard support
  std::unique_ptr<RfbKeySym> m_rfbKeySym;

  // This variable contained previously state of mouse-button and position of cursor.
  unsigned char m_previousMouseState;
  Point m_previousMousePos;

  // scroll bars: vertical and horizontal
  ScrollBar m_sbar;

  // This flag is true if size of window is changed (scroll must be updated).
  bool m_winResize;
  bool m_showVert;
  bool m_showHorz;

  // for scaling
  ScaleManager m_scManager;
  ::int_rectangle m_clientArea;
  int m_fbWidth;
  int m_fbHeight;
  SolidBrush m_brush;

  // frame buffer
  LocalMutex m_bufferLock;
  DibFrameBuffer m_framebuffer;
  // This variable save server dimension.
  // ::int_size of m_framebuffer can be large m_serverDimension.
  ::int_size m_serverDimension;

  // clipboard
  WinClipboard m_clipboard;
  ::string m_strClipboard;
  HWND m_hwndNextViewer;

  bool m_ctrlDown;
  bool m_altDown;
  RemoteViewerCore *m_viewerCore;
  ConnectionConfig *m_conConf;
  bool m_isBackgroundDirty;

public:
  //void doDraw(DeviceContext *dc);
   void doDraw(HDC hdc, const ::int_rectangle & rectangle);
  void scrollProcessing(int fbWidth, int fbHeight);
  void drawBackground(HDC hdc, const RECT & rcMain, const RECT & rcImage);
  void drawImage(HDC hdc, const RECT & src, const RECT & dst);
  void repaint(const ::int_rectangle &  repaintRect);
  void calcClientArea();
};

#endif
