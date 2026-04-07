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

#include "QueryConnectionDialog.h"

#include "remoting_node/resource.h"

QueryConnectionDialog::QueryConnectionDialog(const ::scoped_string & scopedstrPeerAddress,
                                             bool acceptByDefault,
                                             DWORD timeOutInSec)
: m_acceptByDefault(acceptByDefault), m_timeout(timeOutInSec)
{
  m_peerAddress= peerAddress;

  setResourceId(IDD_QUERY_RFB_CONNECTION);
}

QueryConnectionDialog::~QueryConnectionDialog()
{
}

bool QueryConnectionDialog::onInitDialog()
{
  initControls();

  m_peerAddressLabel.setText(m_peerAddress);

  if (m_acceptByDefault) {
    m_acceptButton.setFocus();
  } else {
    m_rejectButton.setFocus();
  }

  setDefaultPushButton(m_acceptByDefault ? IDC_ACCEPT_BUTTON : IDC_REJECT_BUTTON);

  updateTimeoutLabel();

  SetTimer(m_ctrlThis.operating_system_window(), 0, 1000, 0);

  return false;
}

bool QueryConnectionDialog::onNotify(unsigned int controlID, ::lparam data)
{
  return true;
}

bool QueryConnectionDialog::onCommand(unsigned int controlID, unsigned int notificationID)
{
  switch (controlID) {
  case IDC_ACCEPT_BUTTON:
    onAccept();
    break;
  case IDC_REJECT_BUTTON:
    onReject();
    break;
  }
  return true;
}

bool QueryConnectionDialog::onDestroy()
{
  return true;
}

void QueryConnectionDialog::onMessageReceived(unsigned int uMsg, ::wparam wParam, ::lparam lParam)
{
  if (uMsg == WM_TIMER) {
    onTimer();
  }
}

void QueryConnectionDialog::initControls()
{
  HWND window = m_ctrlThis.operating_system_window();

  m_peerAddressLabel.setWindow(GetDlgItem(window, IDC_IP_EDIT));
  m_acceptButton.setWindow(GetDlgItem(window, IDC_ACCEPT_BUTTON));
  m_rejectButton.setWindow(GetDlgItem(window, IDC_REJECT_BUTTON));
  m_timeoutLabel.setWindow(GetDlgItem(window, IDC_TIMEOUT_LABEL));
}

void QueryConnectionDialog::onAccept()
{
  kill(ACCEPT_CHOISE);
}

void QueryConnectionDialog::onReject()
{
  kill(REJECT_CHOISE);
}

void QueryConnectionDialog::onTimer()
{
  if (m_timeout == 0) {
    KillTimer(m_ctrlThis.operating_system_window(), 0);

    if (m_acceptByDefault) {
      onAccept();
    } else {
      onReject();
    }
  } else {
    m_timeout--;

    updateTimeoutLabel();
  }
}

void QueryConnectionDialog::updateTimeoutLabel()
{
  ::string labelText;

  if (m_acceptByDefault) {
    labelText.format(main_subsystem()->string_table()->getString(IDS_AUTO_ACCEPT_CONNECTION_FORMAT), m_timeout);
  } else {
    labelText.format(main_subsystem()->string_table()->getString(IDS_AUTO_REJECT_CONNECTION_FORMAT), m_timeout);
  }

  m_timeoutLabel.setText(labelText);
}
