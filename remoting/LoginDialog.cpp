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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "LoginDialog.h"
#include "NamingDefs.h"
#include "OptionsDialog.h"
#include "remoting/common/remoting.h"
#include "win_system/Shell.h"

LoginDialog::LoginDialog(remoting_impact *viewer)
: BaseDialog(IDD_LOGINDIALOG),
  m_viewer(viewer),
  m_isListening(false)
{
}

LoginDialog::~LoginDialog()
{
}

BOOL LoginDialog::onInitDialog()
{
  setControlById(m_server, IDC_CSERVER);
  setControlById(m_listening, IDC_LISTENING);
  setControlById(m_ok, IDOK);
  updateHistory();
  SetForegroundWindow(get_hwnd());
  m_server.set_focus();
  if (m_isListening) {
    m_listening.enable_window(false);
  }
  return TRUE;
}

void LoginDialog::enableConnect()
{
  ::string str;
  int iSelected = m_server.getSelectedItemIndex();
  if (iSelected == -1) {
    str = m_server.get_text();
    m_ok.enable_window(!str.is_empty());
  } else {
    m_ok.enable_window(true);
  }
}

void LoginDialog::updateHistory()
{
  ConnectionHistory *conHistory;

  ::string currentServer;
  currentServer = m_server.get_text();
  m_server.removeAllItems();
  conHistory = ::remoting::ViewerConfig::getInstance()->getConnectionHistory();
  conHistory->load();
  for (size_t i = 0; i < conHistory->getHostCount(); i++) {
    m_server.insertItem(static_cast<int>(i), conHistory->getHost(i));
  }
  m_server.setText(currentServer);
  if (m_server.getItemsCount()) {
    if (currentServer.is_empty()) {
      m_server.setSelectedItem(0);
    }
    ::string server;
    server = m_server.get_text();
    ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH,
                            server);
    m_connectionConfig.loadFromStorage(&ccsm);
  }
}

void LoginDialog::onConnect()
{
  ConnectionHistory *conHistory = ::remoting::ViewerConfig::getInstance()->getConnectionHistory();

  m_serverHost = m_server.get_text();

  conHistory->load();
  conHistory->addHost(m_serverHost);
  conHistory->save();

  if (!m_serverHost.is_empty()) {
    ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH, m_serverHost);
    m_connectionConfig.saveToStorage(&ccsm);
  }

  m_viewer->newConnection(m_serverHost, m_connectionConfig);
}

void LoginDialog::onConfiguration()
{
  m_viewer->postMessage(remoting_impact::WM_USER_CONFIGURATION);
}

BOOL LoginDialog::onOptions()
{
  OptionsDialog dialog;
  dialog.setConnectionConfig(&m_connectionConfig);
  dialog.set_parent(this);
  if (dialog.showModal() == 1) {
    ::string server;
    server = m_server.get_text();
    if (server.is_empty()) {
      ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH,
                              server);
      m_connectionConfig.saveToStorage(&ccsm);
    }
    return FALSE;
  }
  return TRUE;
}

void LoginDialog::onOrder()
{
  openUrl(StringTable::getString(IDS_URL_LICENSING_FVC));
}

void LoginDialog::openUrl(const ::scoped_string & scopedstrUrl)
{
  // TODO: removed duplicated code (see AboutDialog.h)
  try {
    Shell::open(scopedstrUrl, 0, 0);
  } catch (const SystemException &sysEx) {
    ::string scopedstrMessage;

    scopedstrMessage.formatf(StringTable::getString(IDS_FAILED_TO_OPEN_URL_FORMAT).c_str(), sysEx.get_message().c_str());

    ::remoting::message_box(m_hwnd,
               scopedstrMessage,
               StringTable::getString(IDS_MBC_TVNVIEWER),
               MB_OK | MB_ICONEXCLAMATION);
  }
}

void LoginDialog::setListening(bool isListening)
{
  m_isListening = isListening;
  if (isListening) {
    m_listening.enable_window(false);
  } else {
    m_listening.enable_window(true);
  }
}

void LoginDialog::onListening()
{
  ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH,
                          ".listen");
  m_connectionConfig.loadFromStorage(&ccsm);

  m_listening.enable_window(false);
  m_viewer->startListening(::remoting::ViewerConfig::getInstance()->getListenPort());
}

void LoginDialog::onAbout()
{
  m_viewer->postMessage(remoting_impact::WM_USER_ABOUT);
}

BOOL LoginDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDC_CSERVER:
    switch (notificationID) {
    case CBN_DROPDOWN:
      updateHistory();
      break;

    // select item in ComboBox with ::list_base of history
    case CBN_SELENDOK:
      {
        int selectedItemIndex = m_server.getSelectedItemIndex();
        if (selectedItemIndex < 0) {
          return FALSE;
        }
        //::string server;
        auto server = m_server.getItemText(selectedItemIndex);
        ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH,
                                server);
        m_connectionConfig.loadFromStorage(&ccsm);
        break;
      }
    }

    enableConnect();
    break;

  // click "Connect"
  case IDOK:
    onConnect();
    close_dialog(0);
    break;

  // cancel connection
  case IDCANCEL:
    close_dialog(0);
    break;

  case IDC_BCONFIGURATION:
    onConfiguration();
    break;

  case IDC_BOPTIONS:
    return onOptions();

  case IDC_LISTENING:
    onListening();
    close_dialog(0);
    break;

  case IDC_ORDER_SUPPORT_BUTTON:
    onOrder();
    break;

  case IDC_BABOUT:
    onAbout();
    break;

  default:
    _ASSERT(true);
    return FALSE;
  }
  return TRUE;
}

::string LoginDialog::getServerHost()
{
  return m_serverHost;
}
