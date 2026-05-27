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

#pragma once


#include "subsystem/node/Clipboard.h"
#include "innate_subsystem/framebuffer/DibFramebuffer.h"


#include "ScaleManager.h"
#include "remoting/remoting_rtc/client_config/ConnectionConfig.h"
#include "innate_subsystem/gui/Window.h"
#include "innate_subsystem/gui/ScrollBar.h"
#include "innate_subsystem/drawing/SolidBrush.h"
#include "innate_subsystem/drawing/Graphics.h"
#include "remoting/remoting_rtc/rfb/RfbKeySym.h"
#include "remoting/remoting_rtc/viewer_core/RemoteViewerCore.h"
namespace remoting_client
{
    class toolbar;
    class style;

    class ViewerWindow;
    class DesktopWindow : public ::innate_subsystem::Window,
                          protected ::remoting_rtc::RfbKeySymListener
    {
    public:
        
       
       ::pointer < ViewerWindow > m_pviewerwindow;
        ::string m_strHost;
        //   ::i32 m_iDivisor = 1;
        ::pointer< ::remoting_client::toolbar > m_premotingtoolbar;
        ::pointer< ::remoting_client::style > m_premotingstyle;
        bool m_bShowCursor = false;
        class ::time m_timeStartDesktopWindow;


        DesktopWindow(::subsystem::LogWriter * plogwriter, ::remoting_rtc::ConnectionConfig * pconnectionconfig, ViewerWindow * pviewerwindow);
        ~DesktopWindow() override;

        //virtual void _defer_update_double_buffering();
        void setClipboardData(const ::scoped_string & strText);
        void updateFramebuffer(const ::innate_subsystem::Framebuffer * pframebuffer,
                               const ::i32_rectangle &  rectangleTarget);
        // this function must be called if size of image was changed
        // or the number of bits per pixel
        void setNewFramebuffer(const ::innate_subsystem::Framebuffer * pframebuffer);

        // set scale of image, can -1 = Auto, in percent
        void setScale(::i32 scale);
        // it returns the image width and height considering scale
        ::i32_rectangle getViewerGeometry();
        // it returns the image width and height.
        ::i32_rectangle getFramebufferGeometry();
        // it return size of server frame buffer and pixelsize.
        void getServerGeometry(::i32_rectangle * prectangle, ::i32 * piPixelsize);

        void setConnected();
        void setViewerCore(::remoting_client::RemoteViewerCore *viewerCore);

        // This function set state key "Ctrl", but not send data to server.
        void setCtrlState(const bool ctrlState);
        // This function set state key "Alt", but not send data to server.
        void setAltState(const bool altState);
        // This function return true, if key "::innate_subsystem::Control" is pressed.
        bool getCtrlState() const;
        // This function return true, if key "Alt" is pressed.
        bool getAltState() const;

        // this function sends to remote viewer core
        // key what is pressed or unpressed
        void sendKey(::i32 key, bool pressed);
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
        void onRfbKeySymEvent(::u32 rfbKeySym, bool down) override;

        //
        // Inherited from BaseWindow.
        //
        bool onMessage(::user::enum_message emessage, ::wparam wParam, ::lparam lParam) override;
        //void onPaint(DeviceContext *dc, PAINTSTRUCT *paintStruct);
        void onPaint();
        bool onCreate(void * pCreateStruct) override;
        bool onDrawClipboard() override;
        //bool onEraseBackground(HDC hdc); xxx
        bool onDeadChar(::wparam wParam, ::lparam lParam);
        bool onHScroll(::wparam wParam, ::lparam lParam);
        bool onVScroll(::wparam wParam, ::lparam lParam);
        //bool onKey(::user::enum_message emessage, ::wparam wParam, ::lparam lParam);
       bool onKey(::user::enum_message eusermessage, ::user::enum_key euserkey) override;
        bool onChar(::wparam wParam, ::lparam lParam);
        bool onMouse(::u8 mouseKeys, ::u16 wheelSpeed, const ::i32_point & pointPosition) override;
        bool onMouseEx(::u32 message, ::i32 iButtonMask, ::u16 wspeed, const ::i32_point &pointPosition,
                       bool &bDoDefaultProcessing) override;
        //bool onSize(::wparam wParam, ::lparam lParam);
        void onSize() override;
        bool onDestroy();
       
       //void on_set_cursor_rectangles() override;

        ::i32_point getViewerCoord(long xPos, long yPos);
        void calculateWndSize(bool isChanged);
        void applyScrollbarChanges(bool isChanged, bool isVert, bool isHorz, ::i32 wndWidth, ::i32 wndHeight);

        // This function check pointer to viewer core and send event.
        // If into viewer core throwing exception ::subsystem::Exception, then it catching
        // in this function and logged.
        void sendKeyboardEvent(bool downFlag, ::u32 key);
        void sendPointerEvent(::u8 buttonMask, const ::i32_point &pointPosition);
        void sendCutTextEvent(const ::scoped_string & cutText);

        ::pointer < ::subsystem::LogWriter > m_plogwriter;

        // This variable is true after call CoreEventsAdapter::onConnected().
        bool m_isConnected;

        // keyboard support
        ::remoting_rtc::RfbKeySym * m_rfbKeySym;

        // This variable contained previously state of mouse-button and pointPosition of cursor.
        ::u8 m_previousMouseState;
        ::i32_point m_previousMousePos;

        // scroll bars: vertical and horizontal
        ::innate_subsystem::ScrollBar m_sbar;

        // This flag is true if size of window is changed (scroll must be updated).
        bool m_winResize;
        bool m_showVert;
        bool m_showHorz;

        // for scaling
        ScaleManager m_scManager;
        ::i32_rectangle m_clientArea;
        ::i32 m_fbWidth;
        ::i32 m_fbHeight;
        ::innate_subsystem::SolidBrush m_brush;

        // frame buffer
        lockable_critical_section m_criticalsectionBuffer;
        ::innate_subsystem::DibFramebuffer m_framebuffer;
        // This variable save server dimension.
        // ::i32_size of m_framebuffer can be large m_serverDimension.
        ::i32_size m_serverDimension;

        // clipboard
        ::subsystem::Clipboard m_clipboard;
        ::string m_strClipboard;
        //::operating_system::window m_hwndNextViewer;

        bool m_ctrlDown;
        bool m_altDown;
        ::pointer < ::remoting_client::RemoteViewerCore > m_premoteviewercore;
        ::pointer < ::remoting_rtc::ConnectionConfig > m_pconnectionconfig;
        bool m_isBackgroundDirty;

    public:
        //void doDraw(DeviceContext *dc);
        void onDraw(::innate_subsystem::GraphicsInterface * pgraphics, const ::i32_rectangle & rectangle) override;
       //void onPaint(::innate_subsystem::DeviceContextInterface *pgraphics, const ::i32_rectangle & rectangle) override;
        void scrollProcessing(::i32 fbWidth, ::i32 fbHeight);
        void drawBackground(::innate_subsystem::GraphicsInterface * pgraphics, const ::i32_rectangle & rcMain, const ::i32_rectangle & rcImage);
        void drawImage(::innate_subsystem::GraphicsInterface * pgraphics, const ::i32_rectangle & src, const ::i32_rectangle & dst);
        void repaint(const ::i32_rectangle &  repaintRect);
        void calcClientArea();
    };
} // namespace remoting_client
