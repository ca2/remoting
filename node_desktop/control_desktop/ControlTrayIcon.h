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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "innate_subsystem/gui/NotifyIcon.h"
#include "innate_subsystem/gui/WindowProcHolder.h"
#include "innate_subsystem/drawing/Icon.h"

#include "subsystem/thread/Thread.h"
//#include "subsystem/thread/lockable_critical_section.h"

#include "subsystem/platform/Command.h"
#include "subsystem/platform/MacroCommand.h"

#include "remoting/remoting/node_user_config/ConfigDialog.h"

#include "ControlProxy.h"
#include "remoting/remoting/node/Notificator.h"
#include "ControlApplication.h"
#include "AboutDialog.h"
#include "acme/parallelization/happening.h"


namespace remoting_control_desktop
{
   /**
    * TvnControl application icon class.
    */
   class ControlTrayIcon : public ::innate_subsystem::NotifyIcon /** Inherit tray icon code. */

    //,                       public WindowProcHolder /** To override tray icon window procedure. */
   {
   public:
      /**
       * Creates control tray icon and places it to system tray.
       * @param serverControl proxy to execute methods in TightVNC server process.
       * @param notificator interface to report about errors during execution of remote methods.
       * @param appControl parent control application.
       * @param showAfterCreation determinates if needs to show icon in tray.
       */
      ControlTrayIcon(::remoting_node::Configurator * pconfigurator,
         ControlProxy *serverControl,
                      ::remoting_node::Notificator *notificator,
                      ControlApplication *appControl,
                      bool showAfterCreation);
      /**
       * Destroys tray icon.
       */
      virtual ~ControlTrayIcon();

      /**
       * Synchronizes tray icon and status text with TightVNC server.
       * @remark method shutdowns control application if connection to
       * TightVNC server is lost.
       */
      void syncStatusWithServer();

      // Terminates all function callings and then notifying to the
      // function waitForTermination() to continue.
      void terminate();

      // Wait termination of using a function by windows (e.g. windowProc) and then
      // continue. Don't use this function from thread which call the windowProc()
      // function.
      void waitForTermination();

   protected:
      /**
       * Sets icon state to "not connected to server".
       */
      void setNotConnectedState();

      /**
       * Inherited from WindowProcHolder class.
       *
       * Overrides default tray icon window behavour.
       */
      //virtual LRESULT windowProc(HWND hWnd, ::u32 uMsg, ::wparam wParam, ::lparam lParam, bool *useDefWindowProc);

      bool on_window_procedure(::lresult & lresult, ::u32 message, ::wparam wparam, ::lparam lparam) override;

      /**
       * Handlers of tray icon window events.
       */
      void onNotifyIconRightButtonUp() override;
      void onNotifyIconLeftButtonDown() override;

      /**
       * Tray icon popup menu items scopedstrMessage handlers.
       */
      void onConfigurationMenuItemClick();
      void onDisconnectAllClientsMenuItemClick();
      void onShutdownServerMenuItemClick();
      void onOutgoingConnectionMenuItemClick();
      void onAttachToDispatcher();
      void onAboutMenuItemClick();
      void onCloseControlInterfaceMenuItemClick();

   protected:
      static ::u32 WM_USER_TASKBAR;

   protected:

      ::pointer < ::remoting_node::Configurator > m_pconfigurator;

      // Interface to show error notifications.
      ::pointer < ::remoting_node::Notificator > m_pnotificator;

      // Pointer to control application.
      ::pointer < ControlApplication > m_pcontrolapplication;

      // States of tray icon.
      ::pointer < ::innate_subsystem::IconInterface > m_piconWorking;
      ::pointer < ::innate_subsystem::IconInterface > m_piconIdle;
      ::pointer < ::innate_subsystem::IconInterface > m_piconDisabled;

      // Interface to execute some commands on remote TightVNC server.
      ::pointer < ControlProxy > m_pcontrolproxy;

      // Configuration dialog.
      ::pointer < ::remoting_node::ConfigDialog > m_pconfigdialog;
      // About dialog.
      ::remoting_control_desktop::AboutDialog m_aboutDialog;

      // Last known TightVNC server information.
      ::remoting_control_desktop::ServerInfo m_serverinfoLastKnown;
      // Thread-safety of m_serverinfoLastKnown member.
      lockable_critical_section m_criticalsectionServerInfo;

      // Commands for configuration dialog.
      ::pointer < ::subsystem::Command > m_pcommandUpdateRemoteConfig;
      ::pointer < ::subsystem::Command > m_pcommandUpdateLocalConfig;
      ::pointer < ::subsystem::MacroCommand > m_pmacrocommandApplyChanges;
      ::pointer < ::remoting_node::ControlCommand  > m_pcontrolcommandApplyChanges;

      // This variable is set to true when entering ControlTrayIcon::windowProc(),
      // and is used to prevent from executing that function recursively.
      bool m_inWindowProc;

      ::happening m_happeningEnd;
      bool m_termination;
   };
} // namespace remoting_control_desktop

