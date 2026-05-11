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


#include "subsystem/platform/CommandLine.h"
#include "subsystem/platform/CommandLineFormatException.h"
#include "acme/prototype/geometry2d/rectangle.h"

namespace remoting_control_desktop
{
   class ControlCommandLine : private ::subsystem::CommandLine
   {
   public:
      static ::string_literal CONFIG_APPLICATION;
      static ::string_literal CONFIG_SERVICE;

      static ::string_literal CONTROL_SERVICE;
      static ::string_literal CONTROL_APPLICATION;
      static ::string_literal PASSWORD_FILE;
      static ::string_literal CONFIG_RELOAD;
      static ::string_literal DISCONNECT_ALL;
      static ::string_literal CONNECT;
      static ::string_literal SHUTDOWN;
      static ::string_literal SHARE_PRIMARY;
      static ::string_literal SHARE_RECT;
      static ::string_literal SHARE_DISPLAY;
      static ::string_literal SHARE_WINDOW;
      static ::string_literal SHARE_FULL;
      static ::string_literal SHARE_APP;

      static ::string_literal SET_CONTROL_PASSWORD;
      static ::string_literal SET_PRIMARY_VNC_PASSWORD;
      static ::string_literal CHECK_SERVICE_PASSWORDS;

      static ::string_literal SLAVE_MODE;
      static ::string_literal DONT_ELEVATE;

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
      ::u32 getSharedAppProcessId();

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
      ::u32 m_sharedAppProcessId;
   };
} // namespace remoting_control_desktop

