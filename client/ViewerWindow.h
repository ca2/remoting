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


#include "remoting/client/ConnectionData.h"
#include "DesktopWindow.h"
#include "FileTransferMainDialog.h"
#include "remoting/remoting/client/NamingDefs.h"
#include "OptionsDialog.h"
#include "ScaleManager.h"
#include "ViewerMenu.h"
#include "innate_subsystem/gui/Control.h"
#include "innate_subsystem/gui/Toolbar.h"
//#include "log_writer/LogWriter.h"
#include "remoting/remoting/viewer_core/FileTransferCapability.h"
#include "remoting/remoting/viewer_core/RemoteViewerCore.h"
#include "remoting/remoting/viewer_core/CoreEventsAdapter.h"
#include "remoting/remoting/viewer_core/VncAuthentication.h"
#include "subsystem/node/SystemInformation.h"
#include "subsystem/node/OperatingSystemApplication.h"
#include "subsystem/node/OperatingSystemHook.h"
#include "apex/networking/sockets/http/get_socket.h"


namespace remoting_client
{

   
   class CLASS_DECL_REMOTING_CLIENT ViewerWindow : 
      //public ::innate_subsystem::Window,
      public ::innate_subsystem::Control,
      public ::remoting_client::CoreEventsAdapter,
      public subsystem::OperatingSystemHookListener
   {
   public:
       
       
      ::pointer < ::subsystem::LogWriter > m_plogwriter;

       //::innate_subsystem::Control m_control;

       ::remoting::ConnectionConfigSM m_ccsm;
       ::pointer < ::remoting::ConnectionConfig > m_pconnectionconfig;
       ::pointer < ::subsystem::OperatingSystemApplicationInterface > m_poperatingsystemapplication;
       ::pointer < ::remoting_client::RemoteViewerCore > m_pviewercore;
       ::remoting_client::file_transfer::FileTransferCapability *m_fileTransfer;
       FileTransferMainDialog *m_ftDialog;
       ::pointer < DesktopWindow > m_pdesktopwindow;
       //::wstring m_wstrToolTip;
       ::innate_subsystem::Toolbar m_toolbar;
       ViewerMenu m_menu;
       ::pointer < ConnectionData > m_pconnectiondata;
       ::subsystem::SystemInformation m_sysinf;

       // This variable save ::subsystem::Exception after call onError().
       ::subsystem::Exception m_error;
       // This variable save disconnect-scopedstrMessage after call onDisconnect().
       ::string m_disconnectMessage;

       // Flag is set, if now viewer is in full screen mode
       // bool m_isFullScr;
       // bool m_isMinimizedFromFullScreen = false;
       // It's size of work-area in windowed mode. It is necessary for restore size of window.
       // WINDOWPLACEMENT m_workArea;
       // It's size of optimal size of work-area in windowed mode.
       //::i32_rectangle m_rcNormal;


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
       // private:
       ::array_base<int> m_standardScale;

       ::subsystem::OperatingSystemHook m_operatingsystemhook;
       bool m_hooksEnabledFirstTime;
       //ublic :

        ::pointer < ::sockets::http_client_socket > m_phttpclientsocketNotifyChannel;
        ::pointer < ::remoting_client::keyboard_layout_change > m_pkeyboardlayoutchange;

        ::pointer<::remoting_client::remoting> m_premoting;


        ViewerWindow(
           ::subsystem::OperatingSystemApplicationInterface * papplication,
           ::remoting_client::remoting * premoting,
           ConnectionData * pconnectiondata, 
           ::remoting::ConnectionConfig * pconnectionconfig,
           ::subsystem::LogWriter * plogwriter = nullptr);
        ~ViewerWindow() override;

        void setFileTransfer(::remoting_client::file_transfer::FileTransferCapability *ft);
        void setRemoteViewerCore(::remoting_client::RemoteViewerCore *pCore);

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
      static const int WM_USER_DISCONNECT_NO_CONFIRM = WM_USER + 6;
        static const int WM_USER_SWITCH_FULL_SCREEN_MODE = WM_USER + 1005;
        class ::time m_timeStart;
        //protected:
        static const int TIMER_DESKTOP_STATE = 1;
        static const int TIMER_DESKTOP_STATE_DELAY = 50;

        bool onMessage(::user::enum_message emessage, ::wparam wParam, ::lparam lParam) override;
        //bool onEraseBackground(HDC hdc);

        bool onDisconnect();
      bool _disconnect();
        bool onAuthError(::wparam wParam);
        bool onError();

        bool onFsWarning();
        //bool onSize(::wparam wParam, ::lparam lParam);
        void onSize() override;
        //bool onCreate(LPCREATESTRUCT lps);
       bool onCreate(void *pCreateStruct) override;
        bool onCommand(::u32 controlID, ::u32 notificationID) override;
        //bool onNotify(int idCtrl, LPNMHDR pnmh);
        //bool onSysCommand(::wparam wParam, ::lparam lParam) override;
      bool on_user_system_command(::user::enum_system_command esystemcommand) override;
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
        void onBell() override;
        void onConnecting(int iPhase) override;
        void onConnected(::remoting::RfbOutputGate *output) override;
        void onDisconnect(const ::scoped_string & scopedstrMessage) override;
        void onAuthError(const ::remoting_client::AuthException *exception) override;
        void onError(const ::subsystem::Exception *exception) override;
        void onFramebufferUpdate(const ::innate_subsystem::Framebuffer *pframebuffer, const ::i32_rectangle &  rectangle) override;
        void onFramebufferPropChange(const ::innate_subsystem::Framebuffer *pframebuffer) override;
        void onCutText(const ::scoped_string & cutText) override;

        int translateAccelToTB(int val);
        //void applyScreenChanges(bool isFullScreen);

       ::innate_subsystem::ControlInterface * getControl();

        // function return default rectangle of viewer window:
        // if size of remote screen is more local desktop, then return rectangle of desktop
        // else return rectangle of remote screen + border
        bool onCalculateDefaultSize(::i32_rectangle & rectangleDefaultSize) override;

        void changeCursor(int type);
        void applySettings();
        //::i32_rectangle getFullScreenRect();

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
        //void adjustWindowSize();
        ::string formatWindowName() const;
        void updateKeyState();

        // onHookProc function implementation of HookEventListener base abstract class.
        //virtual LRESULT onHookProc(int code, ::wparam wParam, ::lparam lParam);

       //bool on_keyboard_message()

        bool operating_system_hook_on_keyboard_message(::lresult & lresult, ::user::enum_message emessage, int iVkCode, ::lparam lparam) override;



    };


} //namespace remoting_client

