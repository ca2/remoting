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


#include "AboutDialog.h"
#include "ConfigurationDialog.h"
#include "ConnectionListener.h"
#include "ControlTrayIcon.h"
#include "LoginDialog.h"
#include "OptionsDialog.h"
#include "ViewerCollector.h"
#include "subsystem/node/OperatingSystemApplication.h"

//#include "log_writer/LogWriter.h"
//#include "subsystem/thread/lockable_critical_section.h"

//#include aaa_<map>

namespace remoting_client
{
   class ViewerCollector;
   class ControlTrayIcon;
   class LoginDialog;
   class remoting;
   class remoting_impact : public ::subsystem::OperatingSystemApplication
   {
   public:

      ViewerCollector m_instances;

      // class name of viewer-window
      ::wstring m_viewerWindowClassName;

      // HACCEL m_hAccelTable;
      bool m_isListening;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      AboutDialog m_aboutDialog;
      ConfigurationDialog m_configurationDialog;
      OptionsDialog m_optionsDialog;

      ::pointer < LoginDialog > m_plogindialog;
      ControlTrayIcon *m_pcontroltrayicon;
      ::pointer < ConnectionListener > m_pconnectionlistener;

      // WNDCLASS m_viewerWndClass;

      ::pointer<::remoting_client::remoting> m_premoting;

      remoting_impact(::particle *pparticle, ::remoting_client::remoting *premoting,
         //::hinstance appInstance,
                const ::scoped_string & scopedstrwindowClassName,
                const ::scoped_string & scopedstrviewerWindowClassName);
      virtual ~remoting_impact();

      //
      // show login dialog
      //
      void showLoginDialog();

      //
      // show options for listening mode
      //
      void showListeningOptions();

      //
      // show dialog "About of Viewer"
      //

      void showAboutViewer();

      //
      // show dialog with configuration of viewer
      //
      void showConfiguration();

      // newConnection(...) and startListening(...) always do copy of params (::string,
      // ConnectionData and ConnectionConfig). After call this function can free memory
      // with hostName, pconnectiondata, pconnectionconfig
      void newListeningConnection();
      void newConnection(const ::scoped_string & hostName, ::remoting::ConnectionConfig * pconnectionconfig);
      void newConnection(ConnectionData * pconnectiondata, ::remoting::ConnectionConfig * pconnectionconfig);
      void startListening(int listeningPort);
      void stopListening();

      // Inherited from WindowsApplication
      //void registerWindowClass(WNDCLASS *wndClass);
      //static LRESULT CALLBACK wndProc(HWND hWnd, ::u32 msg, ::wparam wparam, ::lparam lparam);
      //virtual void createWindow(const ::scoped_string & scopedstrClassName);
      virtual void createApplicationMainTask() override;
      //int processMessages();
void defer_check_dead_instance();
      //public:
      // this scopedstrMessage must sended after accepted new listening-connection
      static const int _WM_USER_NEW_LISTENING = WM_USER + 1;

      // this scopedstrMessage need send if you need show login dialog
      static const int _WM_USER_SHOW_LOGIN_DIALOG = WM_USER + 2;

      // this scopedstrMessage need send if you need show configuration dialog
      static const int _WM_USER_CONFIGURATION = WM_USER + 3;

      // this scopedstrMessage need send if you need show about dialog
      static const int _WM_USER_ABOUT = WM_USER + 4;

      // This scopedstrMessage need send if you need reconnect to host.
      // ::lparam contained pointer to ConnectionData.
      static const int WM_USER_RECONNECT = WM_USER + 5;

      // This scopedstrMessage need send if config is changed.
      static const int WM_USER_CONFIGURATION_RELOAD = WM_USER + 6;

      // This timer is used for deleting dead instances of viewer.
      static const int TIMER_DELETE_DEAD_INSTANCE = 1;
      static const int TIMER_DELETE_DEAD_INSTANCE_DELAY = 50;

      //protected:
      void startListeningServer(const int listeningPort);
      void stopListeningServer();
      void restartListeningServer();


      //void postStartDialog(int iStartDialogMessage);
      //void startDialog(int iStartDialogMessage);
      void onMainThreadMessage(::u32 message, ::wparam wparam, ::lparam lparam) override;
      //void registerViewerWindowClass();
      //void unregisterViewerWindowClass();
      //static LRESULT CALLBACK wndProcViewer(HWND hWnd, ::u32 msg, ::wparam wparam, ::lparam lparam);

      void runInstance(ConnectionData * pconnectiondata, ::remoting::ConnectionConfig * pconnectionconfig);
      void runInstance(const ::scoped_string & hostName, ::remoting::ConnectionConfig * pconnectionconfig);

      // This method return true, if login dialog is visible.
      bool isVisibleLoginDialog() const;

      bool onTimer(::wparam idTimer);


      //private:
      void addInstance(ViewerInstance *viewerInstance);


      void run() override;

   };


} // namespace remoting_client


