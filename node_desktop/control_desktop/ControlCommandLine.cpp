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
#include "framework.h"
#include "ControlCommandLine.h"

#include "subsystem/_common_header.h"
#include "subsystem/platform/CommandLine.h"
#include "remoting/remoting/region/RectSerializer.h"
#include "subsystem/platform/StringParser.h"
#include "ConnectStringParser.h"


namespace remoting_control_desktop
{
   string_literal ControlCommandLine::SET_CONTROL_PASSWORD = "-setservicecontrolpass";
   string_literal ControlCommandLine::SET_PRIMARY_VNC_PASSWORD = "-setservicevncpass";
   string_literal ControlCommandLine::CHECK_SERVICE_PASSWORDS = "-checkservicepasswords";

   string_literal ControlCommandLine::CONTROL_SERVICE = "-controlservice";
   string_literal ControlCommandLine::CONTROL_APPLICATION = "-controlapp";
   string_literal ControlCommandLine::PASSWORD_FILE = "-passfile";
   string_literal ControlCommandLine::CONFIG_RELOAD  = "-reload";
   string_literal ControlCommandLine::DISCONNECT_ALL = "-disconnectall";
   string_literal ControlCommandLine::CONNECT = "-connect";
   string_literal ControlCommandLine::SHUTDOWN = "-shutdown";
   string_literal ControlCommandLine::SHARE_PRIMARY = "-shareprimary";
   string_literal ControlCommandLine::SHARE_RECT = "-sharerect";
   string_literal ControlCommandLine::SHARE_DISPLAY = "-sharedisplay";
   string_literal ControlCommandLine::SHARE_WINDOW = "-sharewindow";
   string_literal ControlCommandLine::SHARE_FULL = "-sharefull";
   string_literal ControlCommandLine::SHARE_APP = "-shareapp";

   string_literal ControlCommandLine::CONFIG_APPLICATION = "-configapp";
   string_literal ControlCommandLine::CONFIG_SERVICE = "-configservice";

   string_literal ControlCommandLine::SLAVE_MODE = "-slave";

   string_literal ControlCommandLine::DONT_ELEVATE = "-dontelevate";

   ControlCommandLine::ControlCommandLine()
   : m_displayNumber(0),
     m_sharedAppProcessId(0)
   {
   }

   ControlCommandLine::~ControlCommandLine()
   {
   }

   void ControlCommandLine::parse(const subsystem::CommandLineArguments *cmdArgs)
   {
      ::subsystem::CommandLineFormat fmt[] = {
         { PASSWORD_FILE, ::subsystem::NEEDS_ARG },
         { CONFIG_RELOAD, ::subsystem::NO_ARG },
         { DISCONNECT_ALL, ::subsystem::NO_ARG },
         { CONNECT, ::subsystem::NEEDS_ARG },
         { SHUTDOWN, ::subsystem::NO_ARG },
         { SET_PRIMARY_VNC_PASSWORD, ::subsystem::NEEDS_ARG },
         { SET_CONTROL_PASSWORD, ::subsystem::NEEDS_ARG },
         { CHECK_SERVICE_PASSWORDS, ::subsystem::NO_ARG },
         { SHARE_PRIMARY, ::subsystem::NO_ARG },
         { SHARE_RECT, ::subsystem::NEEDS_ARG },
         { SHARE_DISPLAY, ::subsystem::NEEDS_ARG },
         { SHARE_WINDOW, ::subsystem::NEEDS_ARG },
         { SHARE_FULL, ::subsystem::NO_ARG },
         { SHARE_APP, ::subsystem::NEEDS_ARG },
         { CONTROL_SERVICE, ::subsystem::NO_ARG },
         { CONTROL_APPLICATION, ::subsystem::NO_ARG },
         { CONFIG_APPLICATION, ::subsystem::NO_ARG },
         { CONFIG_SERVICE, ::subsystem::NO_ARG },
         { SLAVE_MODE, ::subsystem::NO_ARG },
         { DONT_ELEVATE, ::subsystem::NO_ARG }
      };

      if (!CommandLine::parse(fmt, sizeof(fmt) / sizeof(::subsystem::CommandLineFormat), cmdArgs)) {
         throw CommandLineFormatException();
      }

      if (hasConfigServiceFlag() && ((int)m_foundKeys.size() > (optionSpecified(DONT_ELEVATE) ? 2 : 1))) {
         throw CommandLineFormatException();
      }

      if (hasConfigAppFlag() && m_foundKeys.size() > 1) {
         throw CommandLineFormatException();
      }

      bool hasPassFile = hasPasswordFile();
      if (hasPassFile) {
         optionSpecified(PASSWORD_FILE, &m_passwordFile);
      }

      if (hasShareRect()) {
         ::string strRect;
         optionSpecified(SHARE_RECT, &strRect);
         parseRectCoordinates(strRect);
      }

      if (hasShareDisplay()) {
         ::string strDisplayNumber;
         optionSpecified(SHARE_DISPLAY, &strDisplayNumber);
         parseDisplayNumber(strDisplayNumber);
      }

      if (hasShareWindow()) {
         optionSpecified(SHARE_WINDOW, &m_windowHeaderName);
      }

      if (hasShareApp()) {
         ::string strAppProcId;
         optionSpecified(SHARE_APP, &strAppProcId);
         parseProcessId(strAppProcId);
      }

      if (hasKillAllFlag() && hasReloadFlag()) {
         throw CommandLineFormatException();
      }

      if (hasConnectFlag()) {
         optionSpecified(CONNECT, &m_connectHostName);
      }

      if ((hasSetVncPasswordFlag() || hasSetControlPasswordFlag()) && m_foundKeys.size() > 1) {
         throw CommandLineFormatException();
      } else {
         optionSpecified(SET_CONTROL_PASSWORD, &m_controlPassword);
         optionSpecified(SET_PRIMARY_VNC_PASSWORD, &m_vncPassword);
      }

      if ((hasControlServiceFlag() || hasControlAppFlag()) && (isSlave()) && (m_foundKeys.size() > 2)) {
         throw CommandLineFormatException();
      }

      bool hasNotSlaveControl = (hasControlServiceFlag() || hasControlAppFlag()) && !isSlave();
      if ((hasNotSlaveControl && !hasPassFile && m_foundKeys.size() > 2) ||
          (hasNotSlaveControl && hasPassFile && m_foundKeys.size() != 3)) {
         throw CommandLineFormatException();
          }

      if (m_foundKeys.size() == 0) {
         throw CommandLineFormatException();
      }
   }

   void ControlCommandLine::getPasswordFile(::string & passwordFile) const
   {
      passwordFile = m_passwordFile;
   }

   bool ControlCommandLine::hasPasswordFile()
   {
      return optionSpecified(PASSWORD_FILE);
   }

   bool ControlCommandLine::hasReloadFlag()
   {
      return optionSpecified(CONFIG_RELOAD);
   }

   bool ControlCommandLine::hasKillAllFlag()
   {
      return optionSpecified(DISCONNECT_ALL);
   }

   bool ControlCommandLine::hasConnectFlag()
   {
      return optionSpecified(CONNECT);
   }

   void ControlCommandLine::getConnectHostName(::string & hostName) const
   {
      hostName = m_connectHostName;
   }

   bool ControlCommandLine::hasShutdownFlag()
   {
      return optionSpecified(SHUTDOWN);
   }

   bool ControlCommandLine::hasSetVncPasswordFlag()
   {
      return optionSpecified(SET_PRIMARY_VNC_PASSWORD);
   }

   bool ControlCommandLine::hasSetControlPasswordFlag()
   {
      return optionSpecified(SET_CONTROL_PASSWORD);
   }

   bool ControlCommandLine::hasConfigAppFlag()
   {
      return optionSpecified(CONFIG_APPLICATION);
   }

   bool ControlCommandLine::hasConfigServiceFlag()
   {
      return optionSpecified(CONFIG_SERVICE);
   }

   bool ControlCommandLine::hasDontElevateFlag()
   {
      return optionSpecified(DONT_ELEVATE);
   }

   bool ControlCommandLine::hasControlServiceFlag()
   {
      return optionSpecified(CONTROL_SERVICE);
   }

   bool ControlCommandLine::hasControlAppFlag()
   {
      return optionSpecified(CONTROL_APPLICATION);
   }

   bool ControlCommandLine::isSlave()
   {
      return optionSpecified(SLAVE_MODE);
   }

   bool ControlCommandLine::hasSharePrimaryFlag()
   {
      return optionSpecified(SHARE_PRIMARY);
   }

   bool ControlCommandLine::hasShareRect()
   {
      return optionSpecified(SHARE_RECT);
   }

   bool ControlCommandLine::hasShareDisplay()
   {
      return optionSpecified(SHARE_DISPLAY);
   }

   bool ControlCommandLine::hasShareWindow()
   {
      return optionSpecified(SHARE_WINDOW);
   }

   bool ControlCommandLine::hasShareFull()
   {
      return optionSpecified(SHARE_FULL);
   }

   bool ControlCommandLine::hasShareApp()
   {
      return optionSpecified(SHARE_APP);
   }

   unsigned char ControlCommandLine::getShareDisplayNumber()
   {
      return m_displayNumber;
   }

   void ControlCommandLine::getShareWindowName(::string & out)
   {
      out = m_windowHeaderName;
   }

   ::int_rectangle ControlCommandLine::getShareRect()
   {
      return m_shareRect;
   }

   ::u32 ControlCommandLine::getSharedAppProcessId()
   {
      return m_sharedAppProcessId;
   }

   bool ControlCommandLine::hasCheckServicePasswords()
   {
      return optionSpecified(CHECK_SERVICE_PASSWORDS);
   }

   ::string ControlCommandLine::getPrimaryVncPassword() const
   {
      return m_vncPassword;
   }

   ::string ControlCommandLine::getControlPassword() const
   {
      return m_controlPassword;
   }

   bool ControlCommandLine::isCommandSpecified()
   {
      return hasKillAllFlag() || hasReloadFlag() || hasSetControlPasswordFlag() ||
             hasSetVncPasswordFlag() || hasConnectFlag() || hasShutdownFlag() ||
             hasSharePrimaryFlag() || hasShareDisplay() || hasShareWindow() ||
             hasShareRect() || hasShareFull() || hasShareApp();
   }

   void ControlCommandLine::parseRectCoordinates(const ::scoped_string & strCoord)
   {
      m_shareRect = ::remoting::RectSerializer::toRect(strCoord);
   }

   void ControlCommandLine::parseDisplayNumber(const ::scoped_string & scopedstrDispNumber)
   {
      if (!MainSubsystem().StringParser().parseByte(scopedstrDispNumber, &m_displayNumber))
         {
         ::string errMess;
         errMess.format("Can't parse the {} argument to a display number", scopedstrDispNumber);
         throw ::subsystem::Exception(errMess);
                                   }
   }

   void ControlCommandLine::parseProcessId(const ::scoped_string & scopedstr)
   {
      if (!MainSubsystem().StringParser().parseUInt(scopedstr, &m_sharedAppProcessId))
         {
         ::string errMess;
         // FIXME: Here the next string must be placed to the resource.
         errMess.format("Can't parse the {} argument to a process id", scopedstr);
         throw ::subsystem::Exception(errMess);
                                   }
   }
} // namespace remoting_control_desktop

