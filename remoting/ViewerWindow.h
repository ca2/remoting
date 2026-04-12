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


#include "remoting/remoting/ConnectionData.h"
#include "DesktopWindow.h"
#include "FileTransferMainDialog.h"
#include "NamingDefs.h"
#include "OptionsDialog.h"
#include "ScaleManager.h"
#include "ViewerMenu.h"
#include "innate_subsystem/Control.h"
#include "innate_subsystem/ToolBar.h"
//#include "log_writer/LogWriter.h"
#include "remoting/remoting_common/viewer_core/FileTransferCapability.h"
#include "remoting/remoting_common/viewer_core/RemoteViewerCore.h"
#include "remoting/remoting_common/viewer_core/CoreEventsAdapter.h"
#include "remoting/remoting_common/viewer_core/VncAuthentication.h"
#include "subsystem/node/SystemInformation.h"
#include "subsystem/node/OperatingSystemApplication.h"
#include "subsystem/node/OperatingSystemHook.h"
#include "apex/networking/sockets/http/get_socket.h"

namespace remoting_remoting
{

    class ViewerWindow : //public ::innate_subsystem::Window,
   public ::innate_subsystem::Control,
                         public ::remoting::CoreEventsAdapter,
                         public subsystem::OperatingSystemHookListener
    {
    public:

        ::pointer < ::sockets::http_client_socket > m_phttpclientsocketNotifyChannel;
        ::pointer < ::remoting_remoting::keyboard_layout_change > m_pkeyboardlayoutchange;



        ViewerWindow(::subsystem::OperatingSystemApplicationInterface *application,
                     ConnectionData *conData, ::remoting::ConnectionConfig *conConf,
                     ::subsystem::LogWriter * plogwriter = 0);
        virtual ~ViewerWindow();

        void setFileTransfer(::remoting::ftp::FileTransferCapability *ft);
        void setRemoteViewerCore(::remoting::RemoteViewerCore *pCore);

        //
        // This function return value of flag m_requiresReconnect.
        //
        bool requiresReconnect() const;

        //
        // This function return value of flag m_stopped.
        //
        bool isStopped() const;

        static const int WM_USER_ERROR = WM_USER + 1;
        static const int WM_USER_STOP = WM_USER + 2;
        static const int WM_USER_DISCONNECT = WM_USER + 3;
        static const int WM_USER_AUTH_ERROR = WM_USER + 4;
        static const int WM_USER_FS_WARNING = WM_USER + 5;
        static const int WM_USER_SWITCH_FULL_SCREEN_MODE = WM_USER + 1005;
        class ::time m_timeStart;
        //protected:
        static const int TIMER_DESKTOP_STATE = 1;
        static const int TIMER_DESKTOP_STATE_DELAY = 50;

        bool onMessage(unsigned int scopedstrMessage, ::wparam wParam, ::lparam lParam);
        bool onEraseBackground(HDC hdc);

        bool onDisconnect();
        bool onAuthError(::wparam wParam);
        bool onError();

        bool onFsWarning();
        bool onSize(::wparam wParam, ::lparam lParam);
        //bool onCreate(LPCREATESTRUCT lps);
       bool onCreate(void *pCreateStruct) override;
        bool onCommand(::wparam wParam, ::lparam lParam);
        bool onNotify(int idCtrl, LPNMHDR pnmh);
        bool onSysCommand(::wparam wParam, ::lparam lParam);
        bool onClose();
        bool onDestroy();
        bool onFocus(::wparam wParam);
        bool onKillFocus(::wparam wParam);
        //bool onTimer(::wparam idTimer);
        void onGoodCursor() override;
        void desktopStateUpdate();
        void commandCtrlAltDel();
        void commandCtrlEsc();
        void commandCtrl();
        void commandAlt();
        void commandToolBar();
        void commandPause();
        void onAbout();
        void commandNewConnection();
        void commandSaveSession();
        void commandScaleIn();
        void commandScaleOut();
        void commandScale100();
        void commandScaleAuto();

        //
        // It is implementation of CoreEventsAdapter functions.
        //
        void onBell();
        void onConnecting(int iPhase) override;
        void onConnected(::remoting::RfbOutputGate *output) override;
        void onDisconnect(const ::scoped_string & scopedstrMessage);
        void onAuthError(const ::remoting::AuthException *exception);
        void onError(const ::subsystem::Exception *exception);
        void onFrameBufferUpdate(const ::innate_subsystem::FrameBuffer *fb, const ::int_rectangle &  rect);
        void onFrameBufferPropChange(const ::innate_subsystem::FrameBuffer *fb);
        void onCutText(const ::scoped_string & cutText);

        int translateAccelToTB(int val);
        //void applyScreenChanges(bool isFullScreen);

       ::innate_subsystem::ControlInterface * getControl();

        // function return default rect of viewer window:
        // if size of remote screen is more local desktop, then return rect of desktop
        // else return rect of remote screen + border
        bool onCalculateDefaultSize(::int_rectangle & rectangleDefaultSize) override;

        ::subsystem::LogWriter * m_plogwriter;

        //::innate_subsystem::Control m_control;

        ::remoting::ConnectionConfigSM m_ccsm;
        ::remoting::ConnectionConfig *m_pconnectionconfig;
        subsystem::OperatingSystemApplicationInterface *m_application;
        ::remoting::RemoteViewerCore *m_viewerCore;
        ::remoting::ftp::FileTransferCapability *m_fileTransfer;
        FileTransferMainDialog *m_ftDialog;
        DesktopWindow m_desktopwindow;
        //::wstring m_wstrToolTip;
        ::innate_subsystem::Toolbar m_toolbar;
        ViewerMenu m_menu;
        ConnectionData *m_conData;
        ::subsystem::SystemInformation m_sysinf;

        // This variable save ::subsystem::Exception after call onError().
        ::subsystem::Exception m_error;
        // This variable save disconnect-scopedstrMessage after call onDisconnect().
        ::string m_disconnectMessage;

        // Flag is set, if now viewer is in full screen mode
        //bool m_isFullScr;
        //bool m_isMinimizedFromFullScreen = false;
        // It's size of work-area in windowed mode. It is necessary for restore size of window.
        //WINDOWPLACEMENT m_workArea;
        // It's size of optimal size of work-area in windowed mode.
        //::int_rectangle m_rcNormal;


        // Flag is set after recv first scopedstrMessage WM_SIZING.
        // bool m_sizeIsChanged;
        // Flag is set, if toolbar is visible.
        bool m_bToolBar;
        // It is scale of viewer window in percent.
        int m_scale;

        // Flag is set after onConnected().
        bool m_isConnected;

        // Flag is set, if instance is requires to reconnect.
        bool m_requiresReconnect;

        // Flag is set, if viewer instance is stopped.
        // Destructor of ViewerWindow may be called, if this flag is true.
        bool m_stopped;
        //private:
        ::array_base<int> m_standardScale;
        void changeCursor(int type);
        void applySettings();
        ::int_rectangle getFullScreenRect();

       void onBeforeFullScreen(bool bRestore) override;
       void onAfterFullScreen(bool bRestore) override;
       void onBeforeUnFullScreen(bool bMinimizing) override;
       void onAfterUnFullScreen(bool bMinimizing) override;
       bool onGetTooltip(int iControl, ::string & strTooltip) override;
        //void setSizeFullScreenWindow();
        //void doFullScr();
        //void doRestoreToFullScreen();
        //void doUnFullScr();
        //void doMinimizeFromFullScreen();

        void doSize();
        void doCommand(int iCommand);
        void showFileTransferDialog();
        void showWindow();
        void enableUserElements();
        bool viewerCoreSettings();
        void dialogConnectionOptions();
        void dialogConnectionInfo();
        void switchFullScreenMode();
        void dialogConfiguration();
        void adjustWindowSize();
        ::string formatWindowName() const;
        void updateKeyState();

        // onHookProc function implementation of HookEventListener base abstract class.
        //virtual LRESULT onHookProc(int code, ::wparam wParam, ::lparam lParam);

       //bool on_keyboard_message()

       virtual bool operating_system_hook_on_keyboard_message(::lresult & lresult, ::user::enum_message emessage, int iVkCode, ::lparam lparam);


       ::subsystem::OperatingSystemHook m_operatingsystemhook;
       bool m_hooksEnabledFirstTime;

    };


} //namespace remoting_remoting

