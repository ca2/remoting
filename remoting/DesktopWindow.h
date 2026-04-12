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

#pragma once


#include "subsystem/node/Clipboard.h"
#include "innate_subsystem/framebuffer/DibFrameBuffer.h"


#include "ScaleManager.h"
#include "remoting/remoting_common/client_config/ConnectionConfig.h"
#include "innate_subsystem/PaintWindow.h"
#include "innate_subsystem/ScrollBar.h"
#include "innate_subsystem/drawing/SolidBrush.h"
#include "innate_subsystem/drawing/Graphics.h"
#include "remoting/remoting_common/rfb/RfbKeySym.h"
#include "remoting/remoting_common/viewer_core/RemoteViewerCore.h"
namespace remoting_remoting
{
    class toolbar;
    class style;

    class ViewerWindow;
    class DesktopWindow : public ::innate_subsystem::PaintWindow,
                          protected ::remoting::RfbKeySymListener
    {
    public:
        ViewerWindow* m_pviewerwindow;
        ::string m_strHost;
        //   int m_iDivisor = 1;
        ::pointer< ::remoting_remoting::toolbar > m_premotingtoolbar;
        ::pointer< ::remoting_remoting::style > m_premotingstyle;
        bool m_bShowCursor = false;
        class ::time m_timeStartDesktopWindow;
        DesktopWindow(::subsystem::LogWriter *logWriter, ::remoting::ConnectionConfig *conConf, ViewerWindow * pviewerwindow);
        virtual ~DesktopWindow();
        virtual void _defer_update_double_buffering();
        void setClipboardData(const ::scoped_string & strText);
        void updateFramebuffer(const ::innate_subsystem::FrameBuffer * pframebuffer,
                               const ::int_rectangle &  dstRect);
        // this function must be called if size of image was changed
        // or the number of bits per pixel
        void setNewFramebuffer(const ::innate_subsystem::FrameBuffer * pframebuffer);

        // set scale of image, can -1 = Auto, in percent
        void setScale(int scale);
        // it returns the image width and height considering scale
        ::int_rectangle getViewerGeometry();
        // it returns the image width and height.
        ::int_rectangle getFrameBufferGeometry();
        // it return size of server frame buffer and pixelsize.
        void getServerGeometry(::int_rectangle *rect, int *pixelsize);

        void setConnected();
        void setViewerCore(::remoting::RemoteViewerCore *viewerCore);

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
        void sendKey(int key, bool pressed);
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
        bool onMessage(unsigned int scopedstrMessage, ::wparam wParam, ::lparam lParam);
        //void onPaint(DeviceContext *dc, PAINTSTRUCT *paintStruct);
        void onPaint();
        bool onCreate(void * pCreateStruct);
        bool onDrawClipboard();
        //bool onEraseBackground(HDC hdc); xxx
        bool onDeadChar(::wparam wParam, ::lparam lParam);
        bool onHScroll(::wparam wParam, ::lparam lParam);
        bool onVScroll(::wparam wParam, ::lparam lParam);
        bool onKey(::wparam wParam, ::lparam lParam);
        bool onChar(::wparam wParam, ::lparam lParam);
        bool onMouse(unsigned char mouseKeys, unsigned short wheelSpeed, const ::int_point & position) override;
        virtual bool onMouseEx(unsigned int message, int iButtonMask, unsigned short wspeed, const ::int_point & position);
        bool onSize(::wparam wParam, ::lparam lParam);
        bool onDestroy();

        ::int_point getViewerCoord(long xPos, long yPos);
        void calculateWndSize(bool isChanged);
        void applyScrollbarChanges(bool isChanged, bool isVert, bool isHorz, int wndWidth, int wndHeight);

        // This function check pointer to viewer core and send event.
        // If into viewer core throwing exception ::subsystem::Exception, then it catching
        // in this function and logged.
        void sendKeyboardEvent(bool downFlag, unsigned int key);
        void sendPointerEvent(unsigned char buttonMask, const ::int_point *position);
        void sendCutTextEvent(const ::scoped_string & cutText);

        ::subsystem::LogWriter *m_plogwriter;

        // This variable is true after call CoreEventsAdapter::onConnected().
        bool m_isConnected;

        // keyboard support
        ::remoting::RfbKeySym * m_rfbKeySym;

        // This variable contained previously state of mouse-button and position of cursor.
        unsigned char m_previousMouseState;
        ::int_point m_previousMousePos;

        // scroll bars: vertical and horizontal
        ::innate_subsystem::ScrollBar m_sbar;

        // This flag is true if size of window is changed (scroll must be updated).
        bool m_winResize;
        bool m_showVert;
        bool m_showHorz;

        // for scaling
        ScaleManager m_scManager;
        ::int_rectangle m_clientArea;
        int m_fbWidth;
        int m_fbHeight;
        ::innate_subsystem::SolidBrush m_brush;

        // frame buffer
        critical_section m_bufferLock;
        ::innate_subsystem::DibFrameBuffer m_framebuffer;
        // This variable save server dimension.
        // ::int_size of m_framebuffer can be large m_serverDimension.
        ::int_size m_serverDimension;

        // clipboard
        ::subsystem::Clipboard m_clipboard;
        ::string m_strClipboard;
        //::operating_system::window m_hwndNextViewer;

        bool m_ctrlDown;
        bool m_altDown;
        ::remoting::RemoteViewerCore *m_viewerCore;
        ::remoting::ConnectionConfig *m_pconnectionconfig;
        bool m_isBackgroundDirty;

    public:
        //void doDraw(DeviceContext *dc);
        void onDraw(::innate_subsystem::GraphicsInterface * pgraphics, const ::int_rectangle & rectangle);
        void scrollProcessing(int fbWidth, int fbHeight);
        void drawBackground(::innate_subsystem::GraphicsInterface * pgraphics, const ::int_rectangle & rcMain, const ::int_rectangle & rcImage);
        void drawImage(::innate_subsystem::GraphicsInterface * pgraphics, const ::int_rectangle & src, const ::int_rectangle & dst);
        void repaint(const ::int_rectangle &  repaintRect);
        void calcClientArea();
    };
} // namespace remoting_remoting
