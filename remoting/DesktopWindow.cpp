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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "DesktopWindow.h"
#include "acme/operating_system/windows/geometry2d.h"
#include "framework.h"
#include "impact_toolbar.h"

#include <dwmapi.h>
DesktopWindow::DesktopWindow(LogWriter *logWriter, ConnectionConfig *conConf) :
    m_logWriter(logWriter), m_clipboard(0), m_showVert(false), m_showHorz(false), m_fbWidth(1), m_fbHeight(1),
    m_winResize(false), m_conConf(conConf), m_brush(RGB(0, 0, 0)),

    m_viewerCore(0), m_ctrlDown(false), m_altDown(false), m_previousMousePos(-1, -1), m_previousMouseState(0),
    m_isBackgroundDirty(false)
{
   m_rfbKeySym = std::unique_ptr<RfbKeySym>(new RfbKeySym(this, m_logWriter));
}

DesktopWindow::~DesktopWindow() {}

void DesktopWindow::setConnected() { m_isConnected = true; }

void DesktopWindow::setViewerCore(RemoteViewerCore *viewerCore) { m_viewerCore = viewerCore; }

bool DesktopWindow::onCreate(LPCREATESTRUCT pcs)
{
   m_sbar.setWindow(getHWnd());
   m_clipboard.setHWnd(getHWnd());
   m_premotingstyle = øallocate ::remoting::style;
   m_premotingtoolbar = øallocate ::remoting::toolbar;
   m_premotingtoolbar->create_impact_toolbar(this, m_premotingstyle);
   m_timeStartDesktopWindow.Now();
   BOOL enable = TRUE; // Use TRUE to force disable, which is counter-intuitive but how the flag works
   HRESULT hr = DwmSetWindowAttribute(::GetParent(getHWnd()), DWMWA_TRANSITIONS_FORCEDISABLED, &enable, sizeof(enable));

   // m_pimpactoolbar->m_pdesktopwindow = this;
   return true;
}


void DesktopWindow::_defer_update_double_buffering()
{

   if (m_hdcBuffer && m_sizeBuffer == m_clientArea.size())
   {

      return;
   }

   if (m_hdcBuffer)
   {
      if (m_hbitmapOld)
      {
         SelectObject(m_hdcBuffer, m_hbitmapOld);
      }
      ::DeleteDC(m_hdcBuffer);
   }

   m_sizeBuffer = m_clientArea.size();

   // 1️⃣ Create memory DC
   m_hdcBuffer = CreateCompatibleDC(m_paintStruct.hdc);

   // 2️⃣ Create 32-bit DIB section (alpha preserved)
   BITMAPINFO bi = {};
   bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bi.bmiHeader.biWidth = m_sizeBuffer.cx;
   bi.bmiHeader.biHeight = -m_sizeBuffer.cy; // top-down
   bi.bmiHeader.biPlanes = 1;
   bi.bmiHeader.biBitCount = 32;
   bi.bmiHeader.biCompression = BI_RGB;

   void *pBits = nullptr;
   m_hbitmapBuffer = CreateDIBSection(m_paintStruct.hdc, &bi, DIB_RGB_COLORS, &pBits, nullptr, 0);
   m_hbitmapOld = (HBITMAP)SelectObject(m_hdcBuffer, m_hbitmapBuffer);
   // 3️⃣ Clear buffer (transparent black)
   ZeroMemory(pBits, m_sizeBuffer.area() * 4);
}

// void DesktopWindow::onPaint(DeviceContext *dc, PAINTSTRUCT *paintStruct)
void DesktopWindow::onPaint()
{

   ::int_rectangle paintRect(m_paintStruct.rcPaint);


   // 5️⃣ Blit to screen (alpha ignored in normal window)


   // Cleanup
   // SelectObject(hdcMem, hOld);
   // DeleteObject(hBmp);
   // DeleteDC(hdcMem);

   // EndPaint(hwnd, &ps);
   if (paintRect.is_empty())
   {

      return;
   }

   if (m_clientArea.is_empty())
   {
      return;
   }


   AutoLock al(&m_bufferLock);


   _defer_update_double_buffering();


   doDraw(m_hdcBuffer, m_paintStruct.rcPaint);

   ::BitBlt(
      m_paintStruct.hdc,
      m_paintStruct.rcPaint.left,
      m_paintStruct.rcPaint.top,
      ::width(m_paintStruct.rcPaint),
      ::height(m_paintStruct.rcPaint),
      m_hdcBuffer,
      m_paintStruct.rcPaint.left,
      m_paintStruct.rcPaint.top,
      SRCCOPY);

}


bool DesktopWindow::onMessage(UINT scopedstrMessage, WPARAM wParam, LPARAM lParam)
{
   switch (scopedstrMessage)
   {
      case WM_HSCROLL:
         return onHScroll(wParam, lParam);
      case WM_VSCROLL:
         return onVScroll(wParam, lParam);
      case WM_ERASEBKGND:
         return onEraseBackground(reinterpret_cast<HDC>(wParam));
      case WM_DEADCHAR:
      case WM_SYSDEADCHAR:
         return onDeadChar(wParam, lParam);
      case WM_CHANGECBCHAIN:
         if ((HWND)wParam == m_hwndNextViewer)
         {
            m_hwndNextViewer = (HWND)lParam;
         }
         else if (m_hwndNextViewer != NULL)
         {
            SendMessage(m_hwndNextViewer, scopedstrMessage, wParam, lParam);
         }
         return true;
      case WM_DRAWCLIPBOARD:
      {
         bool ok = onDrawClipboard();
         SendMessage(m_hwndNextViewer, scopedstrMessage, wParam, lParam);
         return ok;
      }
      case WM_CREATE:
         m_hwndNextViewer = SetClipboardViewer((HWND)wParam);
         return onCreate(reinterpret_cast<LPCREATESTRUCT>(lParam));
      case WM_SIZE:
         return onSize(wParam, lParam);
      case WM_DESTROY:
         return onDestroy();
      case WM_CHAR:
      case WM_SYSCHAR:
         return onChar(wParam, lParam);
      case WM_KEYDOWN:
      case WM_KEYUP:
      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
         return onKey(wParam, lParam);
      case WM_SETCURSOR:
         if (m_bShowCursor || m_timeStartDesktopWindow.elapsed() < 8_s)
         {
            ::SetCursor(LoadCursor(nullptr, IDC_ARROW));
         }
         else
         {
            ::SetCursor(nullptr);
         }
         return true;
      case WM_SETFOCUS:
         m_rfbKeySym->processFocusRestoration();
         return true;
      case WM_KILLFOCUS:
         m_ctrlDown = false;
         m_altDown = false;
         m_rfbKeySym->processFocusLoss();
         return true;
   }
   return false;
}

bool DesktopWindow::onEraseBackground(HDC hdc) { return true; }

bool DesktopWindow::onHScroll(WPARAM wParam, LPARAM lParam)
{
   switch (LOWORD(wParam))
   {
      case SB_THUMBTRACK:
      case SB_THUMBPOSITION:
         m_sbar.setHorzPos(HIWORD(wParam));
         redraw();
         break;
      case SB_LINELEFT:
         m_sbar.moveLeftHorz(ScrollBar::SCROLL_STEP);
         redraw();
         break;
      case SB_PAGELEFT:
         m_sbar.moveLeftHorz();
         redraw();
         break;
      case SB_LINERIGHT:
         m_sbar.moveRightHorz(ScrollBar::SCROLL_STEP);
         redraw();
         break;
      case SB_PAGERIGHT:
         m_sbar.moveRightHorz();
         redraw();
         break;
   }
   return true;
}

bool DesktopWindow::onVScroll(WPARAM wParam, LPARAM lParam)
{
   switch (LOWORD(wParam))
   {
      case SB_THUMBTRACK:
      case SB_THUMBPOSITION:
         m_sbar.setVertPos(HIWORD(wParam));
         redraw();
         break;
      case SB_LINEUP:
         m_sbar.moveDownVert(ScrollBar::SCROLL_STEP);
         redraw();
         break;
      case SB_PAGEUP:
         m_sbar.moveDownVert();
         redraw();
         break;
      case SB_LINEDOWN:
         m_sbar.moveUpVert(ScrollBar::SCROLL_STEP);
         redraw();
         break;
      case SB_PAGEDOWN:
         m_sbar.moveUpVert();
         redraw();
         break;
   }
   return true;
}

bool DesktopWindow::onMouse(unsigned char mouseButtons, unsigned short wheelSpeed, POINT position)
{
   // If mode is "view-only", then skip event.
   if (m_conConf->isViewOnly())
   {
      return true;
   }

   // If viewer isn't connected with server, then skip event.
   if (!m_isConnected)
   {
      return true;
   }


   // If swap of mouse button is enabled, then swap button.
   if (m_conConf->isMouseSwapEnabled() && mouseButtons)
   {
      bool bSecond = !!(mouseButtons & MOUSE_MDOWN);
      bool bThird = !!(mouseButtons & MOUSE_RDOWN);
      mouseButtons &= ~(MOUSE_MDOWN | MOUSE_RDOWN);
      if (bSecond)
      {
         mouseButtons |= MOUSE_RDOWN;
      }
      if (bThird)
      {
         mouseButtons |= MOUSE_MDOWN;
      }
   }

   if (m_premotingtoolbar)
   {
      if (m_premotingtoolbar->_000OnMouse(mouseButtons & MOUSE_LDOWN, position, position))
      {

         return true;
      }
      m_premotingtoolbar->defer_repaint();
   }

   // Translate coordinates from the Viewer Window to Desktop Window.
   POINTS mousePos = getViewerCoord(position.x, position.y);
   Point pos;
   pos.x = mousePos.x;
   pos.y = mousePos.y;

   // If coordinats of point is invalid, then skip event.
   if (pos.x >= 0 && pos.y >= 0)
   {
      unsigned char buttons = mouseButtons;

      // If posititon of mouse isn't change, then don't send event to server.
      if (buttons != m_previousMouseState || !pos.isEqualTo(&m_previousMousePos))
      {
         int wheelMask = MOUSE_WUP | MOUSE_WDOWN;

         // Update previously position of mouse.
         m_previousMouseState = buttons & ~wheelMask;
         m_previousMousePos = pos;

         if ((buttons & wheelMask) == 0)
         {
            // Send position of cursor and state of buttons one time.
            sendPointerEvent(buttons, &pos);
         }
         else
         {
            // Send position of cursor and state of buttons wheelSpeed times.
            for (int i = 0; i < wheelSpeed; i++)
            {
               sendPointerEvent(buttons, &pos);
               sendPointerEvent(buttons & ~wheelMask, &pos);
            }
         } // wheel
      }
   }
   return true;
}

bool DesktopWindow::onKey(WPARAM wParam, LPARAM lParam)
{
   if (!m_conConf->isViewOnly())
   {
      unsigned short virtualKey = static_cast<unsigned short>(wParam);
      unsigned int additionalInfo = static_cast<unsigned int>(lParam);
      static const unsigned int DOWN_FLAG = 0x80000000;
      bool isDown = (additionalInfo & DOWN_FLAG) == 0;

      if (virtualKey == VK_PROCESSKEY)
      {
         bool extended = HIWORD(lParam) & KF_EXTENDED;
         UINT scancode = HIWORD(lParam) & 0xFF;
         if (extended)
         {
            scancode += 0xE000;
         }
         virtualKey = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
      }

      if (virtualKey == VK_CONTROL)
      {
         m_ctrlDown = isDown;
      }

      if (virtualKey == VK_MENU)
      {
         m_altDown = isDown;
      }

      m_rfbKeySym->processKeyEvent(virtualKey, additionalInfo);
   }
   return true;
}

bool DesktopWindow::onChar(WPARAM wParam, LPARAM lParam)
{
   if (!m_conConf->isViewOnly())
   {
      m_rfbKeySym->processCharEvent(static_cast<WCHAR>(wParam), static_cast<unsigned int>(lParam));
   }
   return true;
}

bool DesktopWindow::onDeadChar(WPARAM wParam, LPARAM lParam) { return true; }

bool DesktopWindow::onDrawClipboard()
{
   if (!IsWindowVisible(getHWnd()) || !m_conConf->isClipboardEnabled())
   {
      return false;
   }
   ::string clipboardString;
   if (m_clipboard.getString(clipboardString))
   {

      // if string in clipboard got from server, then don't send him too
      if (clipboardString == m_strClipboard)
      {
         m_strClipboard = "";
         return true;
      }
      m_strClipboard = "";
      sendCutTextEvent(clipboardString);
   }
   return true;
}

void DesktopWindow::setClipboardData(const ::scoped_string &strText)
{
   if (m_conConf->isClipboardEnabled())
   {
      m_clipboard.setString(strText);
      m_strClipboard= strText;
   }
}

void DesktopWindow::doDraw(HDC hdc, const ::int_rectangle &rectangle)
{
   AutoLock al(&m_bufferLock);
   int fbWidth = m_framebuffer.getDimension().cx;
   int fbHeight = m_framebuffer.getDimension().cy;

   if (!fbWidth || !fbHeight)
   {
      ::remoting::Graphics graphics(hdc);

      graphics.fillRect(m_clientArea.left, m_clientArea.top, m_clientArea.right, m_clientArea.bottom, &m_brush);
      return;
   }

   scrollProcessing(fbWidth, fbHeight);

   int iHorzPos = 0;
   int iVertPos = 0;

   if (m_showHorz)
   {
      iHorzPos = m_sbar.getHorzPos();
   }

   if (m_showVert)
   {
      iVertPos = m_sbar.getVertPos();
   }

   m_scManager.setStartPoint(iHorzPos, iVertPos);

   ::int_rectangle src, dst;
   m_scManager.getSourceRect(&src);
   m_scManager.getDestinationRect(&dst);

   int iWidth = m_clientArea.width() - dst.width();
   int iHeight = m_clientArea.height() - dst.height();

   if (iWidth || iHeight)
   {
      drawBackground(hdc, ::windows::as_RECT(m_clientArea), ::windows::as_RECT(::windows::as_RECT(dst)));
   }

   drawImage(hdc, ::windows::as_RECT(src), ::windows::as_RECT(dst));

   if (m_premotingtoolbar)
   {

      //Graphics graphics(hdc);
      //m_premotingtoolbar->__000OnTopDraw(&graphics);
      m_premotingtoolbar->__000OnTopDraw(hdc, rectangle);
   }
}

void DesktopWindow::applyScrollbarChanges(bool isChanged, bool isVert, bool isHorz, int wndWidth, int wndHeight)
{
   if (m_showVert != isVert)
   {
      m_showVert = isVert;
      m_sbar.showVertScroll(m_showVert);
      isChanged = true;
   }

   if (m_showHorz != isHorz)
   {
      m_showHorz = isHorz;
      m_sbar.showHorzScroll(m_showHorz);
      isChanged = true;
   }

   if (isChanged)
   {
      m_scManager.setWindow(::int_rectangle(0, 0, wndWidth, wndHeight));
      if (m_showVert)
      {
         m_sbar.setVertRange(0, m_scManager.getVertPoints(), wndHeight);
      }
      if (m_showHorz)
      {
         m_sbar.setHorzRange(0, m_scManager.getHorzPoints(), wndWidth);
      }
      calcClientArea();
   }
}

void DesktopWindow::calculateWndSize(bool isChanged)
{
   long hScroll = 0;
   long vScroll = 0;
   if (m_showVert)
   {
      vScroll = m_sbar.getVerticalSize();
   }
   if (m_showHorz)
   {
      hScroll = m_sbar.getHorizontalSize();
   }

   int wndWidth = vScroll + m_clientArea.width();
   int wndHeight = hScroll + m_clientArea.height();
   bool isVert = m_scManager.getVertPages(wndHeight);
   bool isHorz = m_scManager.getHorzPages(wndWidth);

   if (isHorz)
   {
      wndHeight -= m_sbar.getHorizontalSize();
   }
   if (isVert)
   {
      wndWidth -= m_sbar.getVerticalSize();
   }

   if (isVert != isHorz)
   {
      // need to recalculate bHorz and bVert
      isVert = m_scManager.getVertPages(wndHeight);
      isHorz = m_scManager.getHorzPages(wndWidth);
      if (isHorz)
      {
         wndHeight -= m_sbar.getHorizontalSize();
      }
      if (isVert)
      {
         wndWidth -= m_sbar.getVerticalSize();
      }
   }
   applyScrollbarChanges(isChanged, isVert, isHorz, wndWidth, wndHeight);
}

void DesktopWindow::scrollProcessing(int fbWidth, int fbHeight)
{
   bool bChanged = false;

   if (m_winResize)
   {
      m_winResize = false;
      bChanged = true;
   }

   if (fbWidth != m_fbWidth || fbHeight != m_fbHeight)
   {
      bChanged = true;
      m_fbWidth = fbWidth;
      m_fbHeight = fbHeight;
      m_scManager.setScreenResolution(fbWidth, fbHeight);
   }

   calculateWndSize(bChanged);
}

void DesktopWindow::drawBackground(HDC hdc, const RECT &rcMain, const RECT &rcImage)
{
   ::remoting::Graphics graphics(hdc);

   // top rectangle
   graphics.fillRect(rcMain.left, rcMain.top, rcMain.right, rcImage.top, &m_brush);
   // left rectangle
   graphics.fillRect(rcMain.left, rcImage.top, rcImage.left, rcImage.bottom, &m_brush);
   // bottom rectangle
   graphics.fillRect(rcMain.left, rcImage.bottom, rcMain.right, rcMain.bottom, &m_brush);
   // right rectangle
   graphics.fillRect(rcImage.right, rcImage.top, rcMain.right, rcImage.bottom, &m_brush);
}

void DesktopWindow::calcClientArea()
{
   if (getHWnd())
   {
      RECT rc;

      getClientRect(&rc);
      m_clientArea = rc;
      m_scManager.setWindow(m_clientArea);
   }
}

void DesktopWindow::drawImage(HDC hdc, const RECT &rectangleSource, const RECT &rectangleTarget)
{
   ::int_rectangle rc_src;
   ::int_rectangle rc_dest;

   ::copy(rc_src, rectangleSource);
   ::copy(rc_dest, rectangleTarget);

   AutoLock al(&m_bufferLock);
   m_framebuffer.setTargetDC(hdc);
   if ((rectangleSource.right - rectangleSource.left) == (rectangleTarget.right - rectangleTarget.left) &&
       (rectangleSource.bottom - rectangleSource.top) == (rectangleTarget.bottom - rectangleTarget.top) &&
       rectangleSource.left == rectangleTarget.left && rectangleSource.right == rectangleTarget.right &&
       rectangleSource.top == rectangleTarget.top && rectangleSource.bottom == rectangleTarget.bottom)
   {
      m_framebuffer.blitFromDibSection(rc_dest);
   }
   else
   {
      m_framebuffer.stretchFromDibSection(rc_dest, rc_src);
   }
}

bool DesktopWindow::onSize(WPARAM wParam, LPARAM lParam)
{
   calcClientArea();
   m_winResize = true;
   return true;
}

bool DesktopWindow::onDestroy() { return true; }

void DesktopWindow::updateFramebuffer(const FrameBuffer *pframebuffer, const ::int_rectangle &dstRect)
{
   // This code doesn't require blocking of m_framebuffer.
   //
   // If in this moment Windows paint frame buffer to screen,
   // then image on viewer is not valid, but next update fix this
   // It is not critical.
   //
   // Size of framebuffer can not changed, because onFrameBufferUpdate()
   // and onFrameBufferPropChange() may be called only from one thread.

   if (!m_framebuffer.copyFrom(dstRect, pframebuffer, dstRect.left, dstRect.top))
   {
      m_logWriter->error("Possible invalide region. ({}, {}), ({}, {})", dstRect.left, dstRect.top, dstRect.right,
                         dstRect.bottom);
      m_logWriter->error("Error in updateFramebuffer (ViewerWindow)");
   }
   repaint(dstRect);
}

void DesktopWindow::setNewFramebuffer(const FrameBuffer *pframebuffer)
{
   ::int_size dimension = pframebuffer->getDimension();
   ::int_size olddimension = m_framebuffer.getDimension();

   bool isBackgroundDirty = dimension.cx < olddimension.cx || dimension.cy < olddimension.cy;

   m_logWriter->debug("Desktop size: {}, {}", dimension.cx, dimension.cy);
   {
      // FIXME: Nested locks should not be used.
      AutoLock al(&m_bufferLock);

      m_serverDimension = dimension;
      if (!dimension.is_empty())
      {
         // the width and height should be aligned to 4
         int alignWidth = (dimension.cx + 3) / 4;
         dimension.cx = alignWidth * 4;
         int alignHeight = (dimension.cy + 3) / 4;
         dimension.cy = alignHeight * 4;
         m_framebuffer.setProperties(dimension, pframebuffer->getPixelFormat(), getHWnd());
         m_framebuffer.setColor(0, 0, 0);
         m_scManager.setScreenResolution(dimension.cx, dimension.cy);
      }
      else
      {
         // If size of remote frame buffer is (0, 0), then fill viewer.
         // Otherwise user might think that everything hangs.
         m_framebuffer.setColor(0, 0, 0);
      }
   }
   if (dimension.is_empty())
   {
      repaint(dimension);
   }
   else
   {
      m_isBackgroundDirty = isBackgroundDirty;
   }
}

void DesktopWindow::repaint(const ::int_rectangle &repaintRect)
{
   ::int_rectangle rect;
   m_scManager.getSourceRect(&rect);
   ::int_rectangle paint = repaintRect;
   paint.intersection(rect);

   // checks what we getted a valid rectangle
   if (paint.width() <= 1 || paint.height() <= 1 || m_isBackgroundDirty)
   {
      m_isBackgroundDirty = false;
      redraw();
      return;
   }
   ::int_rectangle wnd;
   m_scManager.getWndFromScreen(paint, &wnd);
   m_scManager.getDestinationRect(&rect);
   if (wnd.left)
   {
      --wnd.left;
   }
   if (wnd.top)
   {
      --wnd.top;
   }
   if (wnd.right < rect.right)
   {
      ++wnd.right;
   }
   if (wnd.bottom < rect.bottom)
   {
      ++wnd.bottom;
   }
   wnd.intersection(rect);
   redraw(::windows::as_RECT(wnd));
}

void DesktopWindow::setScale(int scale)
{
   AutoLock al(&m_bufferLock);
   m_scManager.setScale(scale);
   m_winResize = true;
   // Invalidate all area of desktop window.
   if (m_hwnd != 0)
   {
      redraw();
   }
}

POINTS DesktopWindow::getViewerCoord(long xPos, long yPos)
{
   ::int_rectangle rect;
   POINTS p;

   m_scManager.getDestinationRect(&rect);
   // it checks this point in the rect
   if (!rect.contains(::int_point(xPos, yPos)))
   {
      p.x = -1;
      p.y = -1;
      return p;
   }
   p = m_scManager.transformDispToScr(xPos, yPos);
   return p;
}

::int_rectangle DesktopWindow::getViewerGeometry()
{
   ::int_rectangle viewerRect;
   viewerRect.set_height(m_scManager.getVertPoints());
   viewerRect.set_width(m_scManager.getHorzPoints());
   if (viewerRect.area() == 0)
   {
      viewerRect.set_width(m_framebuffer.getDimension().cx);
      viewerRect.set_height(m_framebuffer.getDimension().cy);
   }
   return viewerRect;
}

::int_rectangle DesktopWindow::getFrameBufferGeometry()
{
   AutoLock al(&m_bufferLock);
   return m_framebuffer.getDimension();
}

void DesktopWindow::getServerGeometry(::int_rectangle *rect, int *pixelsize)
{
   AutoLock al(&m_bufferLock);
   if (rect != 0)
   {
      *rect = m_serverDimension;
   }
   if (pixelsize != 0)
   {
      *pixelsize = m_framebuffer.getBitsPerPixel();
   }
}

void DesktopWindow::onRfbKeySymEvent(unsigned int rfbKeySym, bool down) { sendKeyboardEvent(down, rfbKeySym); }

void DesktopWindow::setCtrlState(const bool ctrlState) { m_ctrlDown = ctrlState; }

void DesktopWindow::setAltState(const bool altState) { m_altDown = altState; }

bool DesktopWindow::getCtrlState() const { return m_ctrlDown; }

bool DesktopWindow::getAltState() const { return m_altDown; }

void DesktopWindow::sendKey(WCHAR key, bool pressed)
{
   m_rfbKeySym->sendModifier(static_cast<unsigned char>(key), pressed);
}

void DesktopWindow::sendCtrlAltDel() { m_rfbKeySym->sendCtrlAltDel(); }

void DesktopWindow::sendKeyboardEvent(bool downFlag, unsigned int key)
{
   if (m_conConf->isViewOnly())
   {
      return;
   }

   // If pointer to viewer core is 0, then exit.
   if (m_viewerCore == 0)
   {
      return;
   }

   // Trying send keyboard event...
   try
   {
      m_viewerCore->sendKeyboardEvent(downFlag, key);
   }
   catch (const ::remoting::Exception &exception)
   {
      m_logWriter->debug("Error in DesktopWindow::sendKeyboardEvent(): {}", exception.get_message());
   }
}

void DesktopWindow::sendPointerEvent(unsigned char buttonMask, const Point *position)
{
   if (m_conConf->isViewOnly())
   {
      return;
   }

   // If pointer to viewer core is 0, then exit.
   if (m_viewerCore == 0)
   {
      return;
   }

   // Trying send pointer event...
   try
   {
      m_viewerCore->sendPointerEvent(buttonMask, position);
   }
   catch (const ::remoting::Exception &exception)
   {
      m_logWriter->debug("Error in DesktopWindow::sendPointerEvent(): {}", exception.get_message());
   }
}

void DesktopWindow::sendCutTextEvent(const ::scoped_string &cutText)
{
   if (!m_conConf->isClipboardEnabled())
   {
      return;
   }

   // If pointer to viewer core is 0, then exit.
   if (m_viewerCore == 0)
   {
      return;
   }

   // Trying send cut-text event...
   try
   {
      m_viewerCore->sendCutTextEvent(cutText);
   }
   catch (const ::remoting::Exception &exception)
   {
      m_logWriter->debug("Error in DesktopWindow::sendCutTextEvent(): {}", exception.get_message());
   }
}
