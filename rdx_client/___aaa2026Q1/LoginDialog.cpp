// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "LoginDialog.h"
#include "../OptionsDialog.h"
#include "../framework.h"
#include "acme/constant/lightui.h"
#include "innate_subsystem/platform/subsystem.h"
#include "remoting/client/remoting.h"
#include "remoting/remoting_rtc/client/NamingDefs.h"
#include "remoting/remoting_rtc/platform/remoting.h"
#include "subsystem/node/Shell.h"
#include "subsystem/node/SystemException.h"


namespace remoting_client
{
   LoginDialog::LoginDialog(remoting_impact *premotingimpact, ::remoting_client::remoting *premoting) :
       m_premotingimpact(premotingimpact), m_isListening(false), m_premoting(premoting)
   {
      initialize_dialog(IDD_LOGINDIALOG);
   }

   LoginDialog::~LoginDialog() {}

   bool LoginDialog::onInitDialog()
   {
      dialog_item(m_server, IDC_CSERVER);
      dialog_item(m_listening, IDC_LISTENING);
      dialog_item(m_ok, ::innate_subsystem::e_control_id_ok);
      updateHistory();
      setForegroundWindow();
      // SetForegroundWindow(::as_HWND(this->operating_system_window()));
      m_server.setFocus();
      if (m_isListening)
      {
         m_listening.enableWindow(false);
      }
      return true;
   }

   void LoginDialog::enableConnect()
   {
      ::string str;
      int iSelected = m_server.getSelectedItemIndex();
      if (iSelected == -1)
      {
         str = m_server.getText();
         m_ok.enableWindow(!str.is_empty());
      }
      else
      {
         m_ok.enableWindow(true);
      }
   }

   void LoginDialog::updateHistory()
   {
      //::remoting_rtc::ConnectionHistory *conHistory;

      ::string currentServer;
      
      currentServer = m_server.getText();
      
      m_server.removeAllItems();
      
      auto pconnectionhistory = m_premoting->m_pviewerconfig->getConnectionHistory();
      
      pconnectionhistory->load();
      
      m_server.removeAllItems();
      
      for (::collection::index i = 0; i < pconnectionhistory->getHostCount(); i++)
      {
         
         auto iIndex =(::i32) i;
         
         auto strHost = pconnectionhistory->getHost(i);
         
         auto pszDebug = strHost.c_str();
         
         m_server.insertItem(iIndex, strHost);
         
      }
      
      m_server.setText(currentServer);
      
      if (m_server.getItemsCount())
      {

         if (currentServer.is_empty())
         {

            m_server.setSelectedItem(0);
         }

         ::string server;

         server = m_server.getText();

         ::remoting_rtc::ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH, server);

         m_pconnectionconfig->loadFromStorage(&ccsm);
      }
   }


   void LoginDialog::onConnect()
   {

      auto conHistory = m_premoting->m_pviewerconfig->getConnectionHistory();

      m_serverHost = m_server.getText();

      conHistory->load();
      conHistory->addHost(m_serverHost);
      conHistory->save();

      if (m_serverHost.has_character())
      {

         ::remoting_rtc::ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH, m_serverHost);

         m_pconnectionconfig->saveToStorage(&ccsm);
      }

      m_premotingimpact->newConnection(m_serverHost, m_pconnectionconfig);
   }


   void LoginDialog::onConfiguration()
   {

      // m_premotingimpact->postMessage(remoting_impact::WM_USER_CONFIGURATION);
      m_premotingimpact->postMessage(remoting_impact::_WM_USER_CONFIGURATION);
   }

   bool LoginDialog::onOptions()
   {
      OptionsDialog dialog;
      dialog.setConnectionConfig(m_pconnectionconfig);
      dialog.setParent(this);
      if (dialog.showModal() == 1)
      {
         ::string server;
         server = m_server.getText();
         if (server.is_empty())
         {

            ::remoting_rtc::ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH, server);

            m_pconnectionconfig->saveToStorage(&ccsm);
         }

         return false;
      }

      return true;
   }

   void LoginDialog::onOrder() { openUrl(MainSubsystem().StringTable().getString(IDS_URL_LICENSING_FVC)); }

   void LoginDialog::openUrl(const ::scoped_string &scopedstrUrl)
   {
      // TODO: removed duplicated code (see AboutDialog.h)
      try
      {
         MainSubsystem().Shell().open(scopedstrUrl, 0, 0);
      }
      catch (const ::subsystem::SystemException &sysEx)
      {
         ::string strMessage;

         strMessage.runtime_format(MainSubsystem().StringTable().getString(IDS_FAILED_TO_OPEN_URL_FORMAT).c_str(),
                                  sysEx.get_message().c_str());

         MainSubsystem().message_box(operating_system_window(), strMessage,
                                       MainSubsystem().StringTable().getString(IDS_MBC_TVNVIEWER),
                                       ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);
      }
   }

   void LoginDialog::setListening(bool isListening)
   {
      m_isListening = isListening;
      if (isWindow())
      {
         if (isListening)
         {
            m_listening.enableWindow(false);
         }
         else
         {
            m_listening.enableWindow(true);
         }
      }
   }


   void LoginDialog::onListening()
   {

      ::remoting_rtc::ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH, ".listen");
      m_pconnectionconfig->loadFromStorage(&ccsm);

      m_listening.enableWindow(false);
      m_premotingimpact->startListening(m_premoting->m_pviewerconfig->getListenPort());
   }


   void LoginDialog::onAbout() { m_premotingimpact->postMessage(remoting_impact::_WM_USER_ABOUT); }

   bool LoginDialog::onCommand(::u32 controlID, ::u32 notificationID)
   {
      switch (controlID)
      {
         case IDC_CSERVER:
             switch (notificationID) {
                case ::lightui::e_CBN_DROPDOWN:
                     updateHistory();
                     break;

                    // select item in ComboBox with ::list_base of history
                case ::lightui::e_CBN_SELENDOK:
            {
                int selectedItemIndex = m_server.getSelectedItemIndex();
                if (selectedItemIndex < 0) {
                    return false;
                }
                //::string server;
                auto server = m_server.getItemText(selectedItemIndex);
                ::remoting_rtc::ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH,
                                        server);
                m_pconnectionconfig->loadFromStorage(&ccsm);
                break;
            }
            }

            enableConnect();
            break;

            // click "Connect"
         case ::innate_subsystem::e_control_id_ok:
            onConnect();
            closeDialog(0);
            break;

            // cancel connection
         case ::innate_subsystem::e_control_id_cancel:
            closeDialog(0);
            break;

         case IDC_BCONFIGURATION:
            onConfiguration();
            break;

         case IDC_BOPTIONS:
            return onOptions();

         case IDC_LISTENING:
            onListening();
            closeDialog(0);
            break;

         case IDC_ORDER_SUPPORT_BUTTON:
            onOrder();
            break;

         case IDC_BABOUT:
            onAbout();
            break;

         default:
            ASSERT(true);
            return false;
      }
      return true;
   }

   ::string LoginDialog::getServerHost() { return m_serverHost; }
} // namespace remoting_client
