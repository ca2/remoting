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


#include "remoting/util/CommonHeader.h"

#include "gui/BaseDialog.h"
#include "gui/::remoting::Window.h"

class QueryConnectionDialog : public BaseDialog
{
public:
  static const int ACCEPT_CHOISE = 0x0;
  static const int REJECT_CHOISE = 0x1;

public:
  QueryConnectionDialog(const ::scoped_string & scopedstrPeerAddress, bool acceptByDefault,
                        DWORD timeOutInSec);
  virtual ~QueryConnectionDialog();

protected:

  //
  // Inherited from BaseDialog.
  //

  virtual BOOL onInitDialog();
  virtual BOOL onNotify(UINT controlID, LPARAM data);
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  virtual BOOL onDestroy();
  virtual void onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam);

  void initControls();

  void onAccept();
  void onReject();
  void onTimer();

  void updateTimeoutLabel();

protected:
  ::string m_peerAddress;
  bool m_acceptByDefault;
  DWORD m_timeout;

  ::remoting::Window m_peerAddressLabel;
  ::remoting::Window m_timeoutLabel;
  ::remoting::Window m_acceptButton;
  ::remoting::Window m_rejectButton;
};


