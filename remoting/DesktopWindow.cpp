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
#include "framework.h"
#include "DesktopWindow.h"
#include "acme/operating_system/windows/geometry2d.h"
#include "impact_toolbar.h"
#include "ViewerWindow.h"
// #include aaa_<dwmapi.h>


#pragma comment(lib, "dwmapi.lib")

namespace remoting_remoting
{
    DesktopWindow::DesktopWindow(::subsystem::LogWriter *logWriter, ::remoting::ConnectionConfig *conConf, ViewerWindow * pviewerwindow) :
        m_plogwriter(logWriter),m_pviewerwindow(pviewerwindow), m_showVert(false), m_showHorz(false), m_fbWidth(1), m_fbHeight(1),
        m_winResize(false), m_pconnectionconfig(conConf),

        m_viewerCore(0), m_ctrlDown(false), m_altDown(false), m_previousMousePos(-1, -1), m_previousMouseState(0),
        m_isBackgroundDirty(false)
    {
       m_clipboard.initialize_clipboard({});

       m_brush.initialize_solid_brush(color::black);

        m_bShowCursor = true;
       m_rfbKeySym = øraw_new ::remoting::RfbKeySym(this, m_plogwriter);
    }

    DesktopWindow::~DesktopWindow() {}

    void DesktopWindow::setConnected() { m_isConnected = true; }

    void DesktopWindow::setViewerCore(::remoting::RemoteViewerCore *viewerCore) { m_viewerCore = viewerCore; }

    bool DesktopWindow::onCreate(void * pCreateStruct)
    {
       //auto hwnd = (HWND) _HWND();
        m_sbar.set_operating_system_window(operating_system_window());
        m_clipboard.setHWnd(operating_system_window());
        m_premotingstyle = allocateø ::remoting_remoting::style;
        m_premotingtoolbar = allocateø ::remoting_remoting::toolbar;
        m_premotingtoolbar->create_impact_toolbar(this, m_premotingstyle);
        m_timeStartDesktopWindow.Now();
        ///bool enable = true; // Use true to force disable, which is counter-intuitive but how the flag works
        //HRESULT hr = DwmSetWindowAttribute(::GetParent(getHWnd()), DWMWA_TRANSITIONS_FORCEDISABLED, &enable, sizeof(enable));

        // m_pimpactoolbar->m_pdesktopwindow = this;
        return true;
    }

    //
    // void DesktopWindow::_defer_update_double_buffering()
    // {
    //     //
    //     // if (m_hdcBuffer && m_sizeBuffer == m_clientArea.size())
    //     // {
    //     //
    //     //     return;
    //     // }
    //     //
    //     // if (m_hdcBuffer)
    //     // {
    //     //     if (m_hbitmapOld)
    //     //     {
    //     //         SelectObject(m_hdcBuffer, m_hbitmapOld);
    //     //     }
    //     //     ::DeleteDC(m_hdcBuffer);
    //     // }
    //     //
    //     // m_sizeBuffer = m_clientArea.size();
    //     //
    //     // // 1️⃣ Create memory DC
    //     // m_hdcBuffer = CreateCompatibleDC(m_paintStruct.hdc);
    //     //
    //     // // 2️⃣ Create 32-bit DIB section (alpha preserved)
    //     // BITMAPINFO bi = {};
    //     // bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    //     // bi.bmiHeader.biWidth = m_sizeBuffer.cx;
    //     // bi.bmiHeader.biHeight = -m_sizeBuffer.cy; // top-down
    //     // bi.bmiHeader.biPlanes = 1;
    //     // bi.bmiHeader.biBitCount = 32;
    //     // bi.bmiHeader.biCompression = BI_RGB;
    //     //
    //     // void *pBits = nullptr;
    //     // m_hbitmapBuffer = CreateDIBSection(m_paintStruct.hdc, &bi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    //     // m_hbitmapOld = (HBITMAP)SelectObject(m_hdcBuffer, m_hbitmapBuffer);
    //     // // 3️⃣ Clear buffer (transparent black)
    //     // ZeroMemory(pBits, m_sizeBuffer.area() * 4);
    // }

    // // void DesktopWindow::onPaint(DeviceContext *dc, PAINTSTRUCT *paintStruct)
    // void DesktopWindow::onPaint()
    // {
    //
    //     ::int_rectangle paintRect(m_paintStruct.rcPaint);
    //
    //
    //     // 5️⃣ Blit to screen (alpha ignored in normal window)
    //
    //
    //     // Cleanup
    //     // SelectObject(hdcMem, hOld);
    //     // DeleteObject(hBmp);
    //     // DeleteDC(hdcMem);
    //
    //     // EndPaint(hwnd, &ps);
    //     if (paintRect.is_empty())
    //     {
    //
    //         return;
    //     }
    //
    //     if (m_clientArea.is_empty())
    //     {
    //         return;
    //     }
    //
    //
    //     critical_section_lock al(&m_bufferLock);
    //
    //
    //     _defer_update_double_buffering();
    //
    //
    //     doDraw(m_hdcBuffer, m_paintStruct.rcPaint);
    //
    //     ::BitBlt(
    //        m_paintStruct.hdc,
    //        m_paintStruct.rcPaint.left,
    //        m_paintStruct.rcPaint.top,
    //        ::width(m_paintStruct.rcPaint),
    //        ::height(m_paintStruct.rcPaint),
    //        m_hdcBuffer,
    //        m_paintStruct.rcPaint.left,
    //        m_paintStruct.rcPaint.top,
    //        SRCCOPY);
    //
    // }
    //

    bool DesktopWindow::onMessage(unsigned int scopedstrMessage, ::wparam wParam, ::lparam lParam)
    {
        switch (scopedstrMessage)
        {
            case WM_HSCROLL:
                return onHScroll(wParam, lParam);
            case WM_VSCROLL:
                return onVScroll(wParam, lParam);
            case WM_ERASEBKGND:
              return true;
                //return onEraseBackground(reinterpret_cast<HDC>(wParam));
            case WM_DEADCHAR:
            case WM_SYSDEADCHAR:
                return onDeadChar(wParam, lParam);
            // case WM_CHANGECBCHAIN:
            //     if ((HWND)wParam == (HWND) _HWND()NextViewer)
            //     {
            //         (HWND) _HWND()NextViewer = (HWND)lParam;
            //     }
            //     else if ((HWND) _HWND()NextViewer != NULL)
            //     {
            //         SendMessage((HWND) _HWND()NextViewer, scopedstrMessage, wParam, lParam);
            //     }
            //     return true;
            // case WM_DRAWCLIPBOARD:
            // {
            //     bool ok = onDrawClipboard();
            //     SendMessage((HWND) _HWND()NextViewer, scopedstrMessage, wParam, lParam);
            //     return ok;
            // }
            case WM_CREATE:
                //return onCreate(reinterpret_cast<LPCREATESTRUCT>(lParam));
              return false;
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
            {

                return onKey(wParam, lParam);
            }
            // case WM_SETCURSOR:
            //     if (m_bShowCursor || m_timeStartDesktopWindow.elapsed() < 8_s)
            //     {
            //         ::SetCursor(LoadCursor(nullptr, IDC_ARROW));
            //     }
            //     else
            //     {
            //         ::SetCursor(nullptr);
            //     }
            //     return true;
            case WM_SETFOCUS:
            {

                //       // Unregistration of keyboard hook.
                m_pviewerwindow->m_operatingsystemhook.registerKeyboardHook(m_pviewerwindow);
                ////// Switching on ignoring win key.
                setWinKeyIgnore(false);

                m_rfbKeySym->processFocusRestoration();

            }
                return true;
            case WM_KILLFOCUS:
            {
                m_ctrlDown = false;
                m_altDown = false;
                m_rfbKeySym->processFocusLoss();

                //       // Unregistration of keyboard hook.
                m_pviewerwindow->m_operatingsystemhook.unregisterKeyboardHook(m_pviewerwindow);
                ////// Switching on ignoring win key.
                setWinKeyIgnore(true);
            }
                return true;
        }
        return false;
    }

    //bool DesktopWindow::onEraseBackground(HDC hdc) { return true; }

    bool DesktopWindow::onHScroll(::wparam wParam, ::lparam lParam)
    {
        switch (LOWORD(wParam))
        {
            case SB_THUMBTRACK:
            case SB_THUMBPOSITION:
                m_sbar.setHorzPos(HIWORD(wParam));
                redraw();
                break;
            case SB_LINELEFT:
                m_sbar.moveLeftHorz(::innate_subsystem::SCROLL_STEP);
                redraw();
                break;
            case SB_PAGELEFT:
                m_sbar.moveLeftHorz();
                redraw();
                break;
            case SB_LINERIGHT:
                m_sbar.moveRightHorz(::innate_subsystem::SCROLL_STEP);
                redraw();
                break;
            case SB_PAGERIGHT:
                m_sbar.moveRightHorz();
                redraw();
                break;
        }
        return true;
    }

    bool DesktopWindow::onVScroll(::wparam wParam, ::lparam lParam)
    {
        switch (LOWORD(wParam))
        {
            case SB_THUMBTRACK:
            case SB_THUMBPOSITION:
                m_sbar.setVertPos(HIWORD(wParam));
                redraw();
                break;
            case SB_LINEUP:
                m_sbar.moveDownVert(::innate_subsystem::SCROLL_STEP);
                redraw();
                break;
            case SB_PAGEUP:
                m_sbar.moveDownVert();
                redraw();
                break;
            case SB_LINEDOWN:
                m_sbar.moveUpVert(::innate_subsystem::SCROLL_STEP);
                redraw();
                break;
            case SB_PAGEDOWN:
                m_sbar.moveUpVert();
                redraw();
                break;
        }
        return true;
    }

    bool DesktopWindow::onMouseEx(unsigned int uMessage, int iButtonMask, unsigned short wheelSpeed, const ::int_point & point)
    {

        RECT rcClient;
        if (::GetClientRect((HWND) _HWND(), &rcClient))
        {

            if (point.x < -1 || point.y < -1 || point.x >width(rcClient) || point.y> height(rcClient))
            {

                m_bShowCursor = true;

                m_viewerCore->m_fbUpdateNotifier.m_cursorPainter.m_bHideCursor = true;


                ::ReleaseCapture();

                //       // Unregistration of keyboard hook.
                //       m_pviewerwindow->m_winHooks.unregisterKeyboardHook(m_pviewerwindow);
                ////// Switching on ignoring win key.
                //setWinKeyIgnore(true);

            }
            else
            {


                auto hwndCapture = ::GetCapture();

                if (hwndCapture != (HWND) _HWND())
                {

                    ::SetCapture((HWND) _HWND());

                    m_viewerCore->m_fbUpdateNotifier.m_cursorPainter.m_bHideCursor = false;
                    m_bShowCursor = false;

                    //                try {
                    //  // Registration of keyboard hook.
                    //  m_pviewerwindow->m_winHooks.registerKeyboardHook(m_pviewerwindow);
                    //  // Switching off ignoring win key.
                    //  setWinKeyIgnore(false);
                    //} catch (::exception &e) {
                    //  m_plogwriter->error("{}", e.get_message());
                    //}


                }

            }

        }




        if (m_premotingtoolbar)
        {
            if (m_premotingtoolbar->_000OnMouseEx(uMessage, iButtonMask, { point.x, point.y }, { point.x, point.y }))
            {

                if (m_premotingtoolbar->m_bLButtonDown || m_premotingtoolbar->m_bHover)
                {

                    m_bShowCursor = true;
                    m_viewerCore->m_fbUpdateNotifier.m_cursorPainter.m_bHideCursor = true;

                }
                else
                {

                    m_bShowCursor = false;
                    m_viewerCore->m_fbUpdateNotifier.m_cursorPainter.m_bHideCursor = false;

                }


                return true;
            }
            if (m_premotingtoolbar->m_bLButtonDown || m_premotingtoolbar->m_bHover)
            {

                m_bShowCursor = true;
                m_viewerCore->m_fbUpdateNotifier.m_cursorPainter.m_bHideCursor = true;

            }
            else
            {

                m_bShowCursor = false;
                m_viewerCore->m_fbUpdateNotifier.m_cursorPainter.m_bHideCursor = false;

            }
            m_premotingtoolbar->defer_repaint();
        }

        return false;
    }

    bool DesktopWindow::onMouse(unsigned char mouseButtons, unsigned short wheelSpeed, const ::int_point & position)
    {
        if (m_bMinimized)
        {

            return true;

        }
        if (m_premotingtoolbar->m_bLButtonDown)
        {

            return true;

        }
        // If mode is "view-only", then skip event.
        if (m_pconnectionconfig->isViewOnly())
        {
            return true;
        }

        // If viewer isn't connected with server, then skip event.
        if (!m_isConnected)
        {
            return true;
        }


        // If swap of mouse button is enabled, then swap button.
        if (m_pconnectionconfig->isMouseSwapEnabled() && mouseButtons)
        {
            bool bSecond = !!(mouseButtons & ::innate_subsystem::e_mouse_middle);
            bool bThird = !!(mouseButtons & ::innate_subsystem::e_mouse_right);
            mouseButtons &= ~(::innate_subsystem::e_mouse_middle | ::innate_subsystem::e_mouse_right);
            if (bSecond)
            {
                mouseButtons |= ::innate_subsystem::e_mouse_right;
            }
            if (bThird)
            {
                mouseButtons |= ::innate_subsystem::e_mouse_middle;
            }
        }


        auto p = position;

        //p.x *= m_iDivisor;
        //p.y *= m_iDivisor;

        // Translate coordinates from the Viewer Window to Desktop Window.
        auto mousePos = getViewerCoord(p.x, p.y);
        ::int_point pos;
        pos.x = mousePos.x;
        pos.y = mousePos.y;

        // If coordinats of point is invalid, then skip event.
        if (pos.x >= 0 && pos.y >= 0)
        {
            unsigned char buttons = mouseButtons;

            // If posititon of mouse isn't change, then don't send event to server.
            if (buttons != m_previousMouseState || pos != m_previousMousePos)
            {
                int wheelMask = ::innate_subsystem::e_mouse_wheel_up | ::innate_subsystem::e_mouse_wheel_down;

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

    bool DesktopWindow::onKey(::wparam wParam, ::lparam lParam)
    {
       if (m_pviewerwindow->isIconic())
       {

          return false;

       }
        if (!m_pconnectionconfig->isViewOnly())
        {
            unsigned short virtualKey = static_cast<unsigned short>(wParam);
            unsigned int additionalInfo = static_cast<unsigned int>(lParam);
            static const unsigned int DOWN_FLAG = 0x80000000;
            bool isDown = (additionalInfo & DOWN_FLAG) == 0;

            if (virtualKey == VK_PROCESSKEY)
            {
                bool extended = HIWORD(lParam) & KF_EXTENDED;
                unsigned int scancode = HIWORD(lParam) & 0xFF;
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

    bool DesktopWindow::onChar(::wparam wParam, ::lparam lParam)
    {
        if (!m_pconnectionconfig->isViewOnly())
        {
            m_rfbKeySym->processCharEvent(static_cast<WCHAR>(wParam), static_cast<unsigned int>(lParam));
        }
        return true;
    }

    bool DesktopWindow::onDeadChar(::wparam wParam, ::lparam lParam) { return true; }

    bool DesktopWindow::onDrawClipboard()
    {
        if (!isVisible() || !m_pconnectionconfig->isClipboardEnabled())
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
        if (m_pconnectionconfig->isClipboardEnabled())
        {
            m_clipboard.setString(strText);
            m_strClipboard= strText;
        }
    }

    //void DesktopWindow::doDraw(HDC hdc, const ::int_rectangle &rectangle)
   void DesktopWindow::onDraw(::innate_subsystem::GraphicsInterface * pgraphics, const ::int_rectangle &rectangle)
    {
        critical_section_lock al(&m_bufferLock);
        int fbWidth = m_framebuffer.getDimension().cx;
        int fbHeight = m_framebuffer.getDimension().cy;

        if (!fbWidth || !fbHeight)
        {
            //::remoting::Graphics graphics(hdc);

            pgraphics->fillRect(m_clientArea, &m_brush);
            return;
        }

        //int iDivisor = m_iDivisor;

        //if (iDivisor <= 0)
        //{

        //   iDivisor = 1;

        //}

        //scrollProcessing(fbWidth/iDivisor, fbHeight/iDivisor);
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
            drawBackground(pgraphics, ::windows::as_RECT(m_clientArea), ::windows::as_RECT(::windows::as_RECT(dst)));
        }

        drawImage(pgraphics, ::windows::as_RECT(src), ::windows::as_RECT(dst));

        if (m_premotingtoolbar)
        {

            //Graphics graphics(hdc);
            //m_premotingtoolbar->__000OnTopDraw(&graphics);
            m_premotingtoolbar->__000OnTopDraw(pgraphics, rectangle);
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
            if (m_premotingtoolbar)
            {

                m_premotingtoolbar->on_size();

            }
        }

        calculateWndSize(bChanged);
    }

    void DesktopWindow::drawBackground(::innate_subsystem::GraphicsInterface * pgraphics, const ::int_rectangle & rcMain, const ::int_rectangle & rcImage)
    {
        //::remoting::Graphics graphics(hdc);

        // top rectangle
        pgraphics->fillRect({rcMain.left, rcMain.top, rcMain.right, rcImage.top}, &m_brush);
        // left rectangle
        pgraphics->fillRect({rcMain.left, rcImage.top, rcImage.left, rcImage.bottom}, &m_brush);
        // bottom rectangle
        pgraphics->fillRect({rcMain.left, rcImage.bottom, rcMain.right, rcMain.bottom}, &m_brush);
        // right rectangle
        pgraphics->fillRect({rcImage.right, rcImage.top, rcMain.right, rcImage.bottom}, &m_brush);
    }

    void DesktopWindow::calcClientArea()
    {
        if (isWindow())
        {
        //    RECT rc;

            auto rc = getClientRect();
            m_clientArea = rc;
            m_scManager.setWindow(m_clientArea);
        }
    }

    void DesktopWindow::drawImage(::innate_subsystem::GraphicsInterface * pgraphics, const ::int_rectangle & rectangleSource, const ::int_rectangle & rectangleTarget)
    {
        ::int_rectangle rc_src = rectangleSource;
        ::int_rectangle rc_dest = rectangleTarget;

        critical_section_lock al(&m_bufferLock);
        m_framebuffer.setTargetDeviceContext(pgraphics->device_context());
        ::int_rectangle rSource = rectangleSource;

        //int iDivisor = m_iDivisor;

        //if (iDivisor <= 0)
        //{

        //   iDivisor = 1;

        //}

        //rSource *= iDivisor;
        if (rSource == rc_dest)
        {
            m_framebuffer.blitFromDibSection(rc_dest);
        }
        else
        {
            m_framebuffer.stretchFromDibSection(rc_dest, rSource);
        }
    }

    bool DesktopWindow::onSize(::wparam wParam, ::lparam lParam)
    {
        calcClientArea();
        m_winResize = true;
        return true;
    }

    bool DesktopWindow::onDestroy() { return true; }

    void DesktopWindow::updateFramebuffer(const ::innate_subsystem::FrameBuffer *pframebuffer, const ::int_rectangle &dstRect)
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
            m_plogwriter->error("Possible invalide region. ({}, {}), ({}, {})", dstRect.left, dstRect.top, dstRect.right,
                               dstRect.bottom);
            m_plogwriter->error("Error in updateFramebuffer (ViewerWindow)");
        }
        repaint(dstRect);
    }

    void DesktopWindow::setNewFramebuffer(const ::innate_subsystem::FrameBuffer *pframebuffer)
    {
        ::int_size dimension = pframebuffer->getDimension();
        ::int_size olddimension = m_framebuffer.getDimension();

        bool isBackgroundDirty = dimension.cx < olddimension.cx || dimension.cy < olddimension.cy;

        //m_premotingtoolbar->set_size(dimension.width(), m_iDivisor);

        //m_premotingtoolbar->on_size();

        m_plogwriter->debug("Desktop size: {}, {}", dimension.cx, dimension.cy);
        {
            // FIXME: Nested locks should not be used.
            critical_section_lock al(&m_bufferLock);

            m_serverDimension = dimension;
            if (!dimension.is_empty())
            {
                // the width and height should be aligned to 4
                int alignWidth = (dimension.cx + 3) / 4;
                dimension.cx = alignWidth * 4;
                int alignHeight = (dimension.cy + 3) / 4;
                dimension.cy = alignHeight * 4;
                m_framebuffer.setProperties(dimension, pframebuffer->getPixelFormat(), operating_system_window());
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

    void DesktopWindow::repaint(const ::int_rectangle &repaintRectParameter)
    {
        auto repaintRect = repaintRectParameter;
        //repaintRect /= m_iDivisor;
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
        critical_section_lock al(&m_bufferLock);
        m_scManager.setScale(scale);
        m_winResize = true;
        // Invalidate all area of desktop window.
        if ((HWND) _HWND() != 0)
        {
            redraw();
        }
    }

    ::int_point DesktopWindow::getViewerCoord(long xPos, long yPos)
    {
        ::int_rectangle rect;
        ::int_point p;

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
        critical_section_lock al(&m_bufferLock);
        return m_framebuffer.getDimension();
    }

    void DesktopWindow::getServerGeometry(::int_rectangle *rect, int *pixelsize)
    {
        critical_section_lock al(&m_bufferLock);
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

    void DesktopWindow::sendKey(int key, bool pressed)
    {
        m_rfbKeySym->sendModifier(static_cast<unsigned char>(key), pressed);
    }

    void DesktopWindow::sendCtrlAltDel() { m_rfbKeySym->sendCtrlAltDel(); }

    void DesktopWindow::sendKeyboardEvent(bool downFlag, unsigned int key)
    {
        if (m_pconnectionconfig->isViewOnly())
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
        catch (const ::subsystem::Exception &exception)
        {
            m_plogwriter->debug("Error in DesktopWindow::sendKeyboardEvent(): {}", exception.get_message());
        }
    }

    void DesktopWindow::sendPointerEvent(unsigned char buttonMask, const ::int_point *position)
    {
        if (m_pconnectionconfig->isViewOnly())
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
        catch (const ::subsystem::Exception &exception)
        {
            m_plogwriter->debug("Error in DesktopWindow::sendPointerEvent(): {}", exception.get_message());
        }
    }

    void DesktopWindow::sendCutTextEvent(const ::scoped_string &cutText)
    {
        if (!m_pconnectionconfig->isClipboardEnabled())
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
        catch (const ::subsystem::Exception &exception)
        {
            m_plogwriter->debug("Error in DesktopWindow::sendCutTextEvent(): {}", exception.get_message());
        }
    }
}//namespace remoting_remoting
