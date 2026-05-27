// Copyright (C) 2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
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
//#include "acme/operating_system/windows/geometry2d.h"
#include "impact_toolbar.h"
#include "ViewerWindow.h"
#include "innate_subsystem/platform/subsystem.h"
// #include aaa_<dwmapi.h>


//#pragma comment(lib, "dwmapi.lib")

namespace remoting_rfb_client
{
    DesktopWindow::DesktopWindow(::subsystem::LogWriter * plogwriter, ::remoting_rfb::ConnectionConfig *pconnectionconfig, ViewerWindow * pviewerwindow) :
        m_plogwriter(plogwriter),m_pviewerwindow(pviewerwindow), m_showVert(false), m_showHorz(false), m_fbWidth(1), m_fbHeight(1),
        m_winResize(false), m_pconnectionconfig(pconnectionconfig),

        m_premoteviewercore(0), m_ctrlDown(false), m_altDown(false), m_previousMousePos(-1, -1), m_previousMouseState(0),
        m_isBackgroundDirty(false)
    {
       m_clipboard.initialize_clipboard({});

       m_brush.initialize_solid_brush(color::black);

        m_bShowCursor = true;
       m_rfbKeySym = øraw_new ::remoting_rfb::RfbKeySym(this, m_plogwriter);
    }

    DesktopWindow::~DesktopWindow() {}

    void DesktopWindow::setConnected() { m_isConnected = true; }

    void DesktopWindow::setViewerCore(::remoting_rfb_client::RemoteViewerCore *viewerCore) { m_premoteviewercore = viewerCore; }

    bool DesktopWindow::onCreate(void * pCreateStruct)
    {
       //auto hwnd = ::as_HWND(this->operating_system_window());
        m_sbar.set_operating_system_window(operating_system_window());
        m_clipboard.setHWnd(operating_system_window());
        m_premotingstyle = allocateø ::remoting_rfb_client::style;
        m_premotingtoolbar = allocateø ::remoting_rfb_client::toolbar;
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
    //     ::i32_rectangle paintRect(m_paintStruct.rcPaint);
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
    //     critical_section_lock al(&m_criticalsectionBuffer);
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

    bool DesktopWindow::onMessage(::user::enum_message emessage, ::wparam wParam, ::lparam lParam)
    {
        switch (emessage)
        {
           case ::user::e_message_scroll_x:
                return onHScroll(wParam, lParam);
           case ::user::e_message_scroll_y:
                return onVScroll(wParam, lParam);
           case ::user::e_message_erase_background:
              return true;
                //return onEraseBackground(reinterpret_cast<HDC>(wParam));
           case ::user::e_message_dead_char:
           case ::user::e_message_sys_dead_char:
                return onDeadChar(wParam, lParam);
            // case WM_CHANGECBCHAIN:
            //     if ((HWND)wParam == ::as_HWND(this->operating_system_window())NextViewer)
            //     {
            //         ::as_HWND(this->operating_system_window())NextViewer = (HWND)lParam;
            //     }
            //     else if (::as_HWND(this->operating_system_window())NextViewer != NULL)
            //     {
            //         SendMessage(::as_HWND(this->operating_system_window())NextViewer, scopedstrMessage, wParam, lParam);
            //     }
            //     return true;
            // case WM_DRAWCLIPBOARD:
            // {
            //     bool ok = onDrawClipboard();
            //     SendMessage(::as_HWND(this->operating_system_window())NextViewer, scopedstrMessage, wParam, lParam);
            //     return ok;
            // }
           case ::user::e_message_create:
                //return onCreate(reinterpret_cast<LPCREATESTRUCT>(lParam));
              return false;
            //case WM_SIZE:
            //   onSize();
               //return true;
                //return onSize(wParam, lParam);
           case ::user::e_message_destroy:
                return onDestroy();
           case ::user::e_message_char:
           case ::user::e_message_sys_char:
                return onChar(wParam, lParam);
           case ::user::e_message_key_down:
           case ::user::e_message_key_up:
           case ::user::e_message_sys_key_down:
           case ::user::e_message_sys_key_up:
            {
               throw "todo";
                //return onKey(emessage, wParam, lParam);
               return false;
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
           case ::user::e_message_set_focus:
            {

                //       // Unregistration of keyboard hook.
                m_pviewerwindow->m_operatingsystemhook.registerKeyboardHook(m_pviewerwindow);
                ////// Switching on ignoring win key.
                setWinKeyIgnore(false);

                m_rfbKeySym->processFocusRestoration();

            }
                return true;
           case ::user::e_message_kill_focus:
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
        switch (wParam.loword())
        {
            case WIN32_SB_THUMBTRACK:
            case WIN32_SB_THUMBPOSITION:
                m_sbar.setHorzPos(wParam.hiword());
                redraw();
                break;
            case WIN32_SB_LINELEFT:
                m_sbar.moveLeftHorz(::innate_subsystem::SCROLL_STEP);
                redraw();
                break;
            case WIN32_SB_PAGELEFT:
                m_sbar.moveLeftHorz();
                redraw();
                break;
            case WIN32_SB_LINERIGHT:
                m_sbar.moveRightHorz(::innate_subsystem::SCROLL_STEP);
                redraw();
                break;
            case WIN32_SB_PAGERIGHT:
                m_sbar.moveRightHorz();
                redraw();
                break;
        }
        return true;
    }

    bool DesktopWindow::onVScroll(::wparam wParam, ::lparam lParam)
    {
        switch (wParam.loword())
        {
            case WIN32_SB_THUMBTRACK:
            case WIN32_SB_THUMBPOSITION:
                m_sbar.setVertPos(wParam.hiword());
                redraw();
                break;
            case WIN32_SB_LINEUP:
                m_sbar.moveDownVert(::innate_subsystem::SCROLL_STEP);
                redraw();
                break;
            case WIN32_SB_PAGEUP:
                m_sbar.moveDownVert();
                redraw();
                break;
            case WIN32_SB_LINEDOWN:
                m_sbar.moveUpVert(::innate_subsystem::SCROLL_STEP);
                redraw();
                break;
            case WIN32_SB_PAGEDOWN:
                m_sbar.moveUpVert();
                redraw();
                break;
        }
        return true;
    }


   bool DesktopWindow::onMouseEx(::u32 uMessage, ::i32 iButtonMask, ::u16 wheelSpeed,
                                  const ::i32_point &point, bool &bDoDefaultProcessing)
   {
       
      auto rectClient = getClientRect();

      //RECT rcClient;
      //if (::GetClientRect(::as_HWND(this->operating_system_window()), &rcClient))
      if(rectClient.is_set())
      {

         if (point.x < -1 || point.y < -1 || point.x >rectClient.width() || point.y> rectClient.width())
         {

            m_bShowCursor = true;

            m_premoteviewercore->m_pfbupdatenotifier->m_cursorpainter.m_bHideCursor = true;
            
            setMouseCursor(::e_cursor_arrow);

            InnateSubsystem().releaseMouseCapture();
            //       ::ReleaseCapture();

            //       // Unregistration of keyboard hook.
            //       m_pviewerwindow->m_winHooks.unregisterKeyboardHook(m_pviewerwindow);
            ////// Switching on ignoring win key.
            //setWinKeyIgnore(true);

         }
         else
         {
            
            setMouseCursor(::e_cursor_none);

            //auto hwndCapture = ::GetCapture();
               
            auto operatingsystemwindowCapture = InnateSubsystem().getMouseCapture();
            
            auto operatingsystemwindowThis = this->operating_system_window();

            if (operatingsystemwindowCapture != operatingsystemwindowThis)
            {

               //::SetCapture(::as_HWND(this->operating_system_window()));
               InnateSubsystem().setMouseCapture(operatingsystemwindowThis);
               
               m_premoteviewercore->m_pfbupdatenotifier->m_cursorpainter.m_bHideCursor = false;
               
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

         bool bRet = m_premotingtoolbar->_000OnMouseEx(uMessage, iButtonMask, { point.x, point.y }, { point.x, point.y }, bDoDefaultProcessing);

         if (m_premotingtoolbar->m_bLButtonDown || m_premotingtoolbar->m_bHover)
         {
            
            setMouseCursor(::e_cursor_arrow);

            m_bShowCursor = true;
            
            m_premoteviewercore->m_pfbupdatenotifier->m_cursorpainter.m_bHideCursor = true;

         }
         else
         {

            m_bShowCursor = false;
            m_premoteviewercore->m_pfbupdatenotifier->m_cursorpainter.m_bHideCursor = false;

         }

         m_premotingtoolbar->defer_repaint();
         
         if(bRet)
         {
            
            return true;
            
         }
      
      }

      return false;
       
   }

   
   bool DesktopWindow::onMouse(::u8 mouseButtons, ::u16 wheelSpeed, const ::i32_point & pointPosition)
   {
        if (m_pviewerwindow->isMinimized())
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


        auto p = pointPosition;

        //p.x *= m_iDivisor;
        //p.y *= m_iDivisor;

        // Translate coordinates from the Viewer Window to Desktop Window.
        auto mousePos = getViewerCoord(p.x, p.y);
        ::i32_point pos;
        pos.x = mousePos.x;
        pos.y = mousePos.y;

        // If coordinats of point is invalid, then skip event.
        if (pos.x >= 0 && pos.y >= 0)
        {
            ::u8 buttons = mouseButtons;

            // If posititon of mouse isn't change, then don't send event to server.
            if (buttons != m_previousMouseState || pos != m_previousMousePos)
            {
                ::i32 wheelMask = ::innate_subsystem::e_mouse_wheel_up | ::innate_subsystem::e_mouse_wheel_down;

                // Update previously pointPosition of mouse.
                m_previousMouseState = buttons & ~wheelMask;
                m_previousMousePos = pos;

                if ((buttons & wheelMask) == 0)
                {
                    // Send pointPosition of cursor and state of buttons one time.
                    sendPointerEvent(buttons, pos);
                }
                else
                {
                    // Send pointPosition of cursor and state of buttons wheelSpeed times.
                    for (::i32 i = 0; i < wheelSpeed; i++)
                    {
                        sendPointerEvent(buttons, pos);
                        sendPointerEvent(buttons & ~wheelMask, pos);
                    }
                } // wheel
            }
        }
        return true;
    }

    bool DesktopWindow::onKey(::user::enum_message emessage, ::user::enum_key euserkey)
    {
       if (m_pviewerwindow->isIconic())
       {

          return false;

       }
        if (!m_pconnectionconfig->isViewOnly())
        {
           
//           auto keyhappening = InnateSubsystem().keyHappeningFromKeyMessage(emessage, wParam, lParam);
           
           ::user::key_happening keyhappening;
           
           keyhappening.m_emessage = emessage;
           keyhappening.m_euserkey = euserkey;
           keyhappening.m_bDown = emessage == ::user::e_message_key_down
           || emessage == ::user::e_message_sys_key_down;
           
//            ::u16 virtualKey = static_cast<::u16>(wParam);
//            ::u32 additionalInfo = static_cast<::u32>(lParam);
//            static const ::u32 DOWN_FLAG = 0x80000000;
//            bool isDown = (additionalInfo & DOWN_FLAG) == 0;
//
//            if (virtualKey == VK_PROCESSKEY)
//            {
//                bool extended = HIWORD(lParam) & KF_EXTENDED;
//                ::u32 scancode = HIWORD(lParam) & 0xFF;
//                if (extended)
//                {
//                    scancode += 0xE000;
//                }
//                virtualKey = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
//            }
//
//            if (virtualKey == VK_CONTROL)
//            {
//                m_ctrlDown = isDown;
//            }
//
//            if (virtualKey == VK_MENU)
//            {
//                m_altDown = isDown;
//            }
//
//            m_rfbKeySym->processKeyEvent(virtualKey, additionalInfo);
           m_rfbKeySym->processKeyHappening(keyhappening);
        }
        return true;
    }

//bool DesktopWindow::onKey(::user::enum_message emessage, ::wparam wParam, ::lparam lParam)
//{
//   if (m_pviewerwindow->isIconic())
//   {
//
//      return false;
//
//   }
//    if (!m_pconnectionconfig->isViewOnly())
//    {
//       
//       auto keyhappening = InnateSubsystem().keyHappeningFromKeyMessage(emessage, wParam, lParam);
//       
////            ::u16 virtualKey = static_cast<::u16>(wParam);
////            ::u32 additionalInfo = static_cast<::u32>(lParam);
////            static const ::u32 DOWN_FLAG = 0x80000000;
////            bool isDown = (additionalInfo & DOWN_FLAG) == 0;
////
////            if (virtualKey == VK_PROCESSKEY)
////            {
////                bool extended = HIWORD(lParam) & KF_EXTENDED;
////                ::u32 scancode = HIWORD(lParam) & 0xFF;
////                if (extended)
////                {
////                    scancode += 0xE000;
////                }
////                virtualKey = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
////            }
////
////            if (virtualKey == VK_CONTROL)
////            {
////                m_ctrlDown = isDown;
////            }
////
////            if (virtualKey == VK_MENU)
////            {
////                m_altDown = isDown;
////            }
////
////            m_rfbKeySym->processKeyEvent(virtualKey, additionalInfo);
//       m_rfbKeySym->processKeyHappening(keyhappening);
//    }
//    return true;
//}

    bool DesktopWindow::onChar(::wparam wParam, ::lparam lParam)
    {
        if (!m_pconnectionconfig->isViewOnly())
        {
            m_rfbKeySym->processCharEvent((::i32)(wParam.m_wparam), (::u32)(lParam.m_lparam));
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


    //void DesktopWindow::onPaint(::innate_subsystem::DeviceContextInterface* pdevicecontext,
    //    const ::i32_rectangle& rectangle)
    //{

    //    innate_subsystem::Graphics g;

    //    g.initialize_graphics( pdevicecontext);

    //    onDraw(&g, rectangle);

    //}

    //void DesktopWindow::doDraw(HDC hdc, const ::i32_rectangle & rectangle)
   void DesktopWindow::onDraw(::innate_subsystem::GraphicsInterface * pgraphics, const ::i32_rectangle & rectangle)
    {
        critical_section_lock al(&m_criticalsectionBuffer);
        ::i32 fbWidth = m_framebuffer.getDimension().cx;
        ::i32 fbHeight = m_framebuffer.getDimension().cy;

        if (!fbWidth || !fbHeight)
        {
            //::remoting_rfb::Graphics graphics(hdc);

            pgraphics->fillRect(m_clientArea, &m_brush);
            return;
        }

        //::i32 iDivisor = m_iDivisor;

        //if (iDivisor <= 0)
        //{

        //   iDivisor = 1;

        //}

        //scrollProcessing(fbWidth/iDivisor, fbHeight/iDivisor);
        scrollProcessing(fbWidth, fbHeight);

        ::i32 iHorzPos = 0;
        ::i32 iVertPos = 0;

        if (m_showHorz)
        {
            iHorzPos = m_sbar.getHorzPos();
        }

        if (m_showVert)
        {
            iVertPos = m_sbar.getVertPos();
        }

        m_scManager.setStartPoint(iHorzPos, iVertPos);

        ::i32_rectangle src, dst;
        m_scManager.getSourceRect(src);
        m_scManager.getDestinationRect(dst);

        ::i32 iWidth = m_clientArea.width() - dst.width();
        ::i32 iHeight = m_clientArea.height() - dst.height();

        if (iWidth || iHeight)
        {
            drawBackground(pgraphics, m_clientArea, dst);
        }

        drawImage(pgraphics, src, dst);

        if (m_premotingtoolbar)
        {

            //Graphics graphics(hdc);
            //m_premotingtoolbar->__000OnTopDraw(&graphics);
            m_premotingtoolbar->__000OnTopDraw(pgraphics, rectangle);
        }
      
      pgraphics->setBlendModeOn(true);

      pgraphics->fillRect({100, 100, 500, 250}, argb(120, 100, 160, 255));
      //pgraphics->setBlendModeOn(true);
      //pgraphics->fillRect({0, 0, 1024, 768}, argb(160, 100, 160, 255));
      
    }

    void DesktopWindow::applyScrollbarChanges(bool isChanged, bool isVert, bool isHorz, ::i32 wndWidth, ::i32 wndHeight)
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
            m_scManager.setWindow(::i32_rectangle(0, 0, wndWidth, wndHeight));
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

        ::i32 wndWidth = vScroll + m_clientArea.width();
        ::i32 wndHeight = hScroll + m_clientArea.height();
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

    void DesktopWindow::scrollProcessing(::i32 fbWidth, ::i32 fbHeight)
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

    void DesktopWindow::drawBackground(::innate_subsystem::GraphicsInterface * pgraphics, const ::i32_rectangle & rcMain, const ::i32_rectangle & rcImage)
    {
        //::remoting_rfb::Graphics graphics(hdc);

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

    void DesktopWindow::drawImage(::innate_subsystem::GraphicsInterface * pgraphics, const ::i32_rectangle & rectangleSource, const ::i32_rectangle & rectangleTarget)
    {
        ::i32_rectangle rc_src = rectangleSource;
        ::i32_rectangle rc_dest = rectangleTarget;

        critical_section_lock al(&m_criticalsectionBuffer);
        m_framebuffer.setTargetDeviceContext(pgraphics->device_context());
        ::i32_rectangle rSource = rectangleSource;

        //::i32 iDivisor = m_iDivisor;

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

    //bool DesktopWindow::onSize(::wparam wParam, ::lparam lParam)
    void DesktopWindow::onSize()
    {
        calcClientArea();
        m_winResize = true;
       
       m_premotingtoolbar->on_size();

        //return true;
    }

    bool DesktopWindow::onDestroy() { return true; }

//   void DesktopWindow::on_set_cursor_rectangles()
//   {
//      
//      //discard_all_cursor_rectangles();
//      
//      auto r = getClientRect();
//      
//      if(r.is_set())
//      {
//         
//         auto rToolbar = m_premotingtoolbar->m_rectangle;
//         
//         if(rToolbar.is_set())
//         {
//            
//            add_cursor_rectangle(rToolbar, e_cursor_arrow);
//            
//         }
//         
//         r.top = rToolbar.bottom;
//         
//         add_cursor_rectangle(r, e_cursor_none);
//         
//         auto rLeft = rToolbar;
//         
//         rLeft.left = r.left;
//         rLeft.right = rToolbar.left;
//         
//         if(rLeft.is_set())
//         {
//            
//            add_cursor_rectangle(rLeft, e_cursor_none);
//            
//         }
//         
//         auto rRight = rToolbar;
//         
//         rRight.left = rToolbar.right;
//         rRight.right = r.right;
//         
//         if(rRight.is_set())
//         {
//            
//            add_cursor_rectangle(rRight, e_cursor_none);
//            
//         }
//         
//      }
//      
//   }

    void DesktopWindow::updateFramebuffer(const ::innate_subsystem::Framebuffer *pframebuffer, const ::i32_rectangle &rectangleTarget)
    {
        // This code doesn't require blocking of m_framebuffer.
        //
        // If in this moment Windows paint frame buffer to screen,
        // then image on viewer is not valid, but next update fix this
        // It is not critical.
        //
        // Size of pframebuffer can not changed, because onFramebufferUpdate()
        // and onFramebufferPropChange() may be called only from one thread.
        if (!m_framebuffer.copyFrom(rectangleTarget, pframebuffer, rectangleTarget.left, rectangleTarget.top))
        {
            m_plogwriter->error("Possible invalide region. ({}, {}), ({}, {})", rectangleTarget.left, rectangleTarget.top, rectangleTarget.right,
                               rectangleTarget.bottom);
            m_plogwriter->error("Error in updateFramebuffer (ViewerWindow)");
        }
        repaint(rectangleTarget);
    }

    void DesktopWindow::setNewFramebuffer(const ::innate_subsystem::Framebuffer *pframebuffer)
    {
        ::i32_size dimension = pframebuffer->getDimension();
        ::i32_size olddimension = m_framebuffer.getDimension();

        bool isBackgroundDirty = dimension.cx < olddimension.cx || dimension.cy < olddimension.cy;

        //m_premotingtoolbar->set_size(dimension.width(), m_iDivisor);

        m_premotingtoolbar->on_size();
       
       //invalidate_cursor_rectangles();

        m_plogwriter->debug("Desktop size: {}, {}", dimension.cx, dimension.cy);
        {
            // FIXME: Nested locks should not be used.
            critical_section_lock al(&m_criticalsectionBuffer);

            m_serverDimension = dimension;
            if (!dimension.is_empty())
            {
                // the width and height should be aligned to 4
                ::i32 alignWidth = (dimension.cx + 3) / 4;
                dimension.cx = alignWidth * 4;
                ::i32 alignHeight = (dimension.cy + 3) / 4;
                dimension.cy = alignHeight * 4;
                m_framebuffer.setProperties(dimension, pframebuffer->getPixelFormat(), operating_system_window());
                m_framebuffer.setColor(0, 0, 0);
               
               auto width = dimension.cx;
               
               auto height = dimension.cy;
               
               m_scManager.setScreenResolution(width, height);
               
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

    void DesktopWindow::repaint(const ::i32_rectangle &repaintRectParameter)
    {
        auto repaintRect = repaintRectParameter;
        //repaintRect /= m_iDivisor;
        ::i32_rectangle rectangle;
        m_scManager.getSourceRect(rectangle);
        ::i32_rectangle paint = repaintRect;
        paint.intersection(rectangle);

        // checks what we getted a valid rectangle
        if (paint.width() <= 1 || paint.height() <= 1 || m_isBackgroundDirty)
        {
            m_isBackgroundDirty = false;
            redraw();
            return;
        }
        ::i32_rectangle wnd;
        m_scManager.getWndFromScreen(paint, wnd);
        m_scManager.getDestinationRect(rectangle);
        if (wnd.left)
        {
            --wnd.left;
        }
        if (wnd.top)
        {
            --wnd.top;
        }
        if (wnd.right < rectangle.right)
        {
            ++wnd.right;
        }
        if (wnd.bottom < rectangle.bottom)
        {
            ++wnd.bottom;
        }
        wnd.intersection(rectangle);
        redraw(wnd);
    }

    void DesktopWindow::setScale(::i32 scale)
    {
        critical_section_lock al(&m_criticalsectionBuffer);
        m_scManager.setScale(scale);
        m_winResize = true;
        // Invalidate all area of desktop window.
        if (isWindow())
        {
            redraw();
        }
    }

    ::i32_point DesktopWindow::getViewerCoord(long xPos, long yPos)
    {
        ::i32_rectangle rectangle;
        ::i32_point p;

        m_scManager.getDestinationRect(rectangle);
        // it checks this point in the rectangle
        if (!rectangle.contains(::i32_point(xPos, yPos)))
        {
            p.x = -1;
            p.y = -1;
            return p;
        }
        p = m_scManager.transformDispToScr(xPos, yPos);
        return p;
    }

    ::i32_rectangle DesktopWindow::getViewerGeometry()
    {
        ::i32_rectangle viewerRect;
        viewerRect.set_height(m_scManager.getVertPoints());
        viewerRect.set_width(m_scManager.getHorzPoints());
        if (viewerRect.area() == 0)
        {
            viewerRect.set_width(m_framebuffer.getDimension().cx);
            viewerRect.set_height(m_framebuffer.getDimension().cy);
        }
        return viewerRect;
    }

    ::i32_rectangle DesktopWindow::getFramebufferGeometry()
    {
        critical_section_lock al(&m_criticalsectionBuffer);
        return m_framebuffer.getDimension();
    }

    void DesktopWindow::getServerGeometry(::i32_rectangle * prectangle, ::i32 *piPixelsize)
    {
        critical_section_lock al(&m_criticalsectionBuffer);
        if (prectangle)
        {
            *prectangle = m_serverDimension;
        }
        if (piPixelsize)
        {
            *piPixelsize = m_framebuffer.getBitsPerPixel();
        }
    }

    void DesktopWindow::onRfbKeySymEvent(::u32 rfbKeySym, bool down) { sendKeyboardEvent(down, rfbKeySym); }

    void DesktopWindow::setCtrlState(const bool ctrlState) { m_ctrlDown = ctrlState; }

    void DesktopWindow::setAltState(const bool altState) { m_altDown = altState; }

    bool DesktopWindow::getCtrlState() const { return m_ctrlDown; }

    bool DesktopWindow::getAltState() const { return m_altDown; }

    void DesktopWindow::sendKey(::i32 key, bool pressed)
    {
        m_rfbKeySym->sendModifier((::user::enum_key)(::u8)(key), pressed);
    }

    void DesktopWindow::sendCtrlAltDel() { m_rfbKeySym->sendCtrlAltDel(); }

    void DesktopWindow::sendKeyboardEvent(bool downFlag, ::u32 key)
    {
        if (m_pconnectionconfig->isViewOnly())
        {
            return;
        }

        // If pointer to viewer core is 0, then exit.
        if (m_premoteviewercore == 0)
        {
            return;
        }

        // Trying send keyboard event...
        try
        {
            m_premoteviewercore->sendKeyboardEvent(downFlag, key);
        }
        catch (const ::subsystem::Exception &exception)
        {
            m_plogwriter->debug("Error in DesktopWindow::sendKeyboardEvent(): {}", exception.get_message());
        }
    }

    void DesktopWindow::sendPointerEvent(::u8 buttonMask, const ::i32_point &pointPosition)
    {
        if (m_pconnectionconfig->isViewOnly())
        {
            return;
        }

        // If pointer to viewer core is 0, then exit.
        if (m_premoteviewercore == 0)
        {
            return;
        }

        // Trying send pointer event...
        try
        {
            m_premoteviewercore->sendPointerEvent(buttonMask, pointPosition);
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
        if (m_premoteviewercore == 0)
        {
            return;
        }

        // Trying send cut-text event...
        try
        {
            m_premoteviewercore->sendCutTextEvent(cutText);
        }
        catch (const ::subsystem::Exception &exception)
        {
            m_plogwriter->debug("Error in DesktopWindow::sendCutTextEvent(): {}", exception.get_message());
        }
    }
}//namespace remoting_rfb_client
