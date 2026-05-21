// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "ControlTrayIcon.h"
#include "OutgoingConnectionDialog.h"
#include "ControlApplication.h"

#include "UpdateRemoteConfigCommand.h"
#include "DisconnectAllCommand.h"
#include "ShutdownCommand.h"
#include "MakeRfbConnectionCommand.h"
#include "remoting/remoting/node/ControlCommand.h"
#include "UpdateLocalConfigCommand.h"
#include "application.h"

#include "remoting/node_desktop/ServerApplication.h"
#include "apex/innate_ui/innate_ui.h"
#include "apex/innate_ui/menu.h"
#include "innate_subsystem/platform/ResourceLoader.h"
#include "innate_subsystem/platform/subsystem.h"
#include "subsystem/platform/StringTable.h"
#include "subsystem/platform/MacroCommand.h"

#include "remoting/node_desktop/resource.h"

//#include aaa_<crtdbg.h>

namespace remoting_control_desktop
{
   ::u32 ControlTrayIcon::WM_USER_TASKBAR;

   ControlTrayIcon::ControlTrayIcon(::remoting_node::Configurator * pconfigurator, ControlProxy *pcontrolproxy,
                                    ::remoting_node::Notificator *notificator,
                                    ControlApplication *appControl,
                                    bool showAfterCreation)
   : //NotifyIcon(showAfterCreation),
      m_pconfigurator(pconfigurator),
     m_pcontrolproxy(pcontrolproxy), m_pnotificator(notificator),
     m_pcontrolapplication(appControl),
     m_inWindowProc(false),
     m_termination(false)
   {
initialize(::system());
      initialize_notify_icon(showAfterCreation);
      auto presourceloader = InnateSubsystem().ResourceLoader();

      m_piconWorking = presourceloader->loadIconByIntResource(IDI_CONNECTED);
      m_piconIdle = presourceloader->loadIconByIntResource(IDI_IDLE);
      m_piconDisabled = presourceloader->loadIconByIntResource(IDI_DISABLED);

      //setWindowProcHolder(this);

      // Prepare commands for configration dialog.
      m_pcommandUpdateRemoteConfig = new UpdateRemoteConfigCommand(m_pconfigurator,m_pcontrolproxy);
      m_pcommandUpdateLocalConfig = new UpdateLocalConfigCommand(m_pconfigurator,m_pcontrolproxy);
      m_pmacrocommandApplyChanges = allocateø ::subsystem::MacroCommand();
      m_pmacrocommandApplyChanges->addCommand(m_pcommandUpdateRemoteConfig);
      // m_pmacrocommandApplyChanges->addCommand(m_pcommandUpdateLocalConfig);
      m_pcontrolcommandApplyChanges = allocateø ::remoting_node::ControlCommand(m_pmacrocommandApplyChanges, m_pnotificator);

      // Create config dialog.
      m_pconfigdialog = allocateø ::remoting_node::ConfigDialog(m_pconfigurator);
      m_pconfigdialog->setConfigReloadCommand(m_pcontrolcommandApplyChanges);

      // Default icon state.
      setNotConnectedState();

      // Update status.
      syncStatusWithServer();

      ///WM_USER_TASKBAR = RegisterWindowMessage("TaskbarCreated");

   }


   ControlTrayIcon::~ControlTrayIcon()
   {
      //delete m_pconfigdialog;

      //delete m_pcontrolcommandApplyChanges;
      //delete m_pmacrocommandApplyChanges;
      //delete m_pcommandUpdateLocalConfig;
      //delete m_pcommandUpdateRemoteConfig;

      //delete m_piconDisabled;
      //delete m_piconIdle;
      //delete m_piconWorking;
   }


   bool ControlTrayIcon::on_window_procedure(::lresult & lresult, ::u32 message, ::wparam wparam, ::lparam lparam)
   {


   // }
   //
   // LRESULT ControlTrayIcon::windowProc(HWND hWnd, ::u32 uMsg, ::wparam wParam, ::lparam lParam,
   //                                     bool *useDefWindowProc)
   //{
      // if (m_inWindowProc) {
      //    // This call is recursive, do not do any real work.
      //    *useDefWindowProc = true;
      //    return 0;
      // }
      // if (m_termination) {
      //    m_happeningEnd.set_happening();
      //    *useDefWindowProc = true;
      //    return 0;
      // }
      //
      // // Make sure to reset it back to false before leaving this function for any
      // // reason (check all return statements, exceptions should not happen here).
      // m_inWindowProc = true;

      switch (message) {
         case WM_USER + 1:
            // switch (lparam) {
            // case WM_RBUTTONUP:
            // {
            //    onRightButtonUp();
            //    lresult = 0;
            //    return true;
            // }
            //       break;
            // case WM_LBUTTONDOWN:
            // {
            //    onLeftButtonDown();
            //    lresult = 0;
            //    return true;
            // }
            //       break;
            // } // switch (lParam)
            break;
         default:
            if (message == WM_USER_TASKBAR) {
               hide();
               lresult = 0;
               return true;
            }
            //*useDefWindowProc = true;
      }

      //m_inWindowProc = false;
      return false;
   }

void ControlTrayIcon::trackPopupMenu()
{

   auto pmenu = system()->innate_ui()->load_menu_from_resource(IDR_TRAYMENU);
   
   pmenu->track_popup_menu(operating_system_window(), [this](int iActionId)
      {

         onAction(iActionId);

      });
   
////   HMENU hRoot = LoadMenu(GetModuleHandle(0), MAKEINTRESOURCE(IDR_TRAYMENU));
////   HMENU hMenu = GetSubMenu(hRoot, 0);
////
////   SetMenuDefaultItem(hMenu, ID_CONFIGURATION, false);
////
////   if (m_pcontrolapplication->m_slaveModeEnabled) {
////      RemoveMenu(hMenu, ID_CLOSE_CONTROL_INTERFACE, MF_BYCOMMAND);
////   }
////
////   POINT pos;
////
////   if (!GetCursorPos(&pos)) {
////      pos.x = pos.y = 0;
////   }
////
////   ///SetForegroundWindow(operating_system_window());
////   ///
////   setForegroundWindow();
////
////   int action = TrackPopupMenu(hMenu,
////                               TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON,
////                               pos.x, pos.y, 0, ::as_HWND(operating_system_window()), NULL);
////
//   // onAction(action);
   
}

   void ControlTrayIcon::onNotifyIconRightButtonUp()
   {
      
      trackPopupMenu();

   }

void ControlTrayIcon::onAction(int action)
{
   
   switch (action) {
      case ID_KILLCLIENTS:
         onDisconnectAllClientsMenuItemClick();
         break;
      case ID_SHUTDOWN_SERVICE:
         onShutdownServerMenuItemClick();
         break;
      case ID_CONFIGURATION:
         onConfigurationMenuItemClick();
         break;
      case ID_OUTGOING_CONN:
         onOutgoingConnectionMenuItemClick();
         break;
      case ID_ABOUT_REMOTING_MENUITEM:
         onAboutMenuItemClick();
         break;
      case ID_CLOSE_CONTROL_INTERFACE:
         onCloseControlInterfaceMenuItemClick();
         break;
   }

   
}

   void ControlTrayIcon::onNotifyIconLeftButtonDown()
   {
      onConfigurationMenuItemClick();
   }

   void ControlTrayIcon::onConfigurationMenuItemClick()
   {

      auto papp = MainSubsystem().m_papplicationSubsystem;

      papp->removeModelessDialog(m_pconfigdialog->operating_system_window());

      bool isConnectedToService = false;

      try {
         isConnectedToService = m_pcontrolproxy->getServerInfo().m_serviceFlag;
      } catch (...) {
         return;
      }

      m_pconfigurator->setServiceFlag(isConnectedToService);

      // Copy running remoting config to our global server config.
      if (!m_pconfigdialog->isCreated()) {
         UpdateLocalConfigCommand updateLocalConfigCommand(m_pconfigurator, m_pcontrolproxy);

         ::remoting_node::ControlCommand safeCommand(&updateLocalConfigCommand, m_pcontrolapplication);

         safeCommand.onRunCommand();

         if (!safeCommand.executionResultOk()) {
            return;
         }
      }

      // Show dialog.
      m_pconfigdialog->setServiceFlag(isConnectedToService);
      m_pconfigdialog->show();

      //::cast < ::remoting_node_desktop::application > papp = m_papplication;

      //auto pserverapp = papp->m_pserverapplication;

      papp->addModelessDialog(m_pconfigdialog->operating_system_window());
   }

   void ControlTrayIcon::onDisconnectAllClientsMenuItemClick()
   {
      DisconnectAllCommand unsafeCommand(m_pcontrolproxy);

      ::remoting_node::ControlCommand safeCommand(&unsafeCommand, m_pnotificator);

      safeCommand.onRunCommand();
   }

   void ControlTrayIcon::onShutdownServerMenuItemClick()
   {
      // Promt user if any client is connected to rfb server.

      // FIXME: Bad way to determinate connected clients.
      bool someoneConnected = (getIcon() == m_piconWorking);

      if (someoneConnected) {
         ::remoting_control_desktop::ServerInfo serverInfo = {0};

         {
            critical_section_lock l(&m_criticalsectionServerInfo);

            serverInfo = m_serverinfoLastKnown;
         }

         ::string userMessage;

         ::u32 stringId = serverInfo.m_serviceFlag ? IDS_TVNSERVER_SERVICE : IDS_TVNSERVER_APP;

         userMessage.runtime_format(
           MainSubsystem().StringTable().getString(IDS_SHUTDOWN_NOTIFICATION_FORMAT),
           MainSubsystem().StringTable().getString(stringId));

         if (MainSubsystem().message_box(
           operating_system_window(),
           userMessage,
           MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
           ::user::e_message_box_yes_no | ::user::e_message_box_icon_question) == ::innate_subsystem::e_control_id_no) {
            return;
           }
      }

      // Shutdown Remoting Node.

      ShutdownCommand unsafeCommand(m_pcontrolproxy);

      ::remoting_node::ControlCommand safeCommand(&unsafeCommand, m_pnotificator);

      safeCommand.onRunCommand();
   }

   void ControlTrayIcon::onOutgoingConnectionMenuItemClick()
   {
      OutgoingConnectionDialog connDialog;

      if (connDialog.showModal() == ::innate_subsystem::e_control_id_ok) {
         MakeRfbConnectionCommand unsafeCommand(
           m_pcontrolproxy,
           connDialog.getConnectString(),
           connDialog.isViewOnly());

         ::remoting_node::ControlCommand safeCommand(&unsafeCommand, m_pnotificator);

         safeCommand.onRunCommand();
      }
   }

   void ControlTrayIcon::onAboutMenuItemClick()
   {
      m_aboutDialog.show();

      auto papp = MainSubsystem().m_papplicationSubsystem;

      papp->addModelessDialog(m_aboutDialog.operating_system_window());
   }

   void ControlTrayIcon::onCloseControlInterfaceMenuItemClick()
   {
      m_pcontrolapplication->shutdown();
   }

   void ControlTrayIcon::syncStatusWithServer()
   {
      try {
         // Get Remoting Node info.
         auto serverinfo = m_pcontrolproxy->getServerInfo();
         ::list_base<RfbClientInfo *> clients;
         m_pcontrolproxy->getClientsList(&clients);

         // Change icon status.
         if (clients.size() > 0) {
            setIcon(m_piconWorking);
         } else if (serverinfo.m_acceptFlag) {
            setIcon(m_piconIdle);
         } else {
            setIcon(m_piconDisabled);
         }

         setText(serverinfo.m_statusText);

         // Cleanup.
         for (::list_base<RfbClientInfo *>::iterator it = clients.begin(); it != clients.end(); it++) {
            delete *it;
         }

         {
            critical_section_lock l(&m_criticalsectionServerInfo);

            m_serverinfoLastKnown = serverinfo;
         }
      } catch (::io_exception &) {
         setNotConnectedState();
      } catch (::subsystem::Exception &) {
         ASSERT(false);
      } // try / catch.
   }

   void ControlTrayIcon::setNotConnectedState()
   {
      setIcon(m_piconDisabled);
      setText(MainSubsystem().StringTable().getString(IDS_CONTROL_CLIENT_NOT_CONNECTED));
   }

   void ControlTrayIcon::setThreadToFinish()
   {
      m_termination = true;
      // Forcing window scopedstrMessage
      postMessage(WM_USER + 1, 0, 0);
   }

   void ControlTrayIcon::waitForTermination()
   {
      
      m_happeningEnd.wait();

   }


} // namespace remoting_control_desktop

