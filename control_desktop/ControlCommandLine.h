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


#include "subsystem/CommandLine.h"
#include "subsystem/CommandLineFormatException.h"
#include "acme/prototype/geometry2d/rectangle.h"


class ControlCommandLine : private ::subsystem::CommandLine
{
public:
  static const char CONFIG_APPLICATION[];
  static const char CONFIG_SERVICE[];

  static const char CONTROL_SERVICE[];
  static const char CONTROL_APPLICATION[];
  static const char PASSWORD_FILE[];
  static const char CONFIG_RELOAD[];
  static const char DISCONNECT_ALL[];
  static const char CONNECT[];
  static const char SHUTDOWN[];
  static const char SHARE_PRIMARY[];
  static const char SHARE_RECT[];
  static const char SHARE_DISPLAY[];
  static const char SHARE_WINDOW[];
  static const char SHARE_FULL[];
  static const char SHARE_APP[];

  static const char SET_CONTROL_PASSWORD[];
  static const char SET_PRIMARY_VNC_PASSWORD[];
  static const char CHECK_SERVICE_PASSWORDS[];

  static const char SLAVE_MODE[];
  static const char DONT_ELEVATE[];

public:
  ControlCommandLine();
  virtual ~ControlCommandLine();

  void parse(const ::subsystem::CommandLineArguments *cmdArgs);

  void getPasswordFile(::string & passwordFile) const;
  bool hasPasswordFile();
  bool hasReloadFlag();
  bool hasKillAllFlag();
  bool hasConnectFlag();
  void getConnectHostName(::string & hostName) const;
  bool hasShutdownFlag();
  bool hasSetVncPasswordFlag();
  bool hasSetControlPasswordFlag();
  bool hasCheckServicePasswords();
  bool hasConfigAppFlag();
  bool hasConfigServiceFlag();
  bool hasControlServiceFlag();
  bool hasControlAppFlag();
  bool hasDontElevateFlag();
  bool isSlave();

  bool hasSharePrimaryFlag();
  bool hasShareRect();
  bool hasShareDisplay();
  bool hasShareWindow();
  bool hasShareFull();
  bool hasShareApp();
  unsigned char getShareDisplayNumber();
  void getShareWindowName(::string & out);
  ::int_rectangle getShareRect();
  unsigned int getSharedAppProcessId();

  ::string getPrimaryVncPassword() const;
  ::string getControlPassword() const;

  bool isCommandSpecified();

private:
  void parseRectCoordinates(const ::scoped_string & strCoord);
  void parseDisplayNumber(const ::scoped_string & strDispNumber);
  void parseProcessId(const ::scoped_string & str);

  ::string m_vncPassword;
  ::string m_controlPassword;

  ::string m_connectHostName;
  ::string m_dispatcherSpec;
  ::string m_passwordFile;

  ::int_rectangle m_shareRect;
  unsigned char m_displayNumber;
  ::string m_windowHeaderName;
  unsigned int m_sharedAppProcessId;
};


