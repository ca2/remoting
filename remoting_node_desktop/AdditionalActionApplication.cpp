// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#include "AdditionalActionApplication.h"
#include "TvnServerHelp.h"

#include "remoting/remoting_common/util/CommandLine.h"

#include "remoting/remoting_common/win_system/Workstation.h"
#include "remoting/remoting_common/win_system/WinCommandLineArgs.h"

const TCHAR AdditionalActionApplication::LOCK_WORKSTATION_KEY[] = "-lockworkstation";
const TCHAR AdditionalActionApplication::LOGOUT_KEY[] = "-logout";

AdditionalActionApplication::AdditionalActionApplication(HINSTANCE hInstance,
                                                         const ::scoped_string & scopedstrwindowClassName,
                                                         const ::scoped_string & scopedstrCommandLine)
: LocalWindowsApplication(hInstance, windowClassName),
  m_commandLine(commandLine)
{
}

AdditionalActionApplication::~AdditionalActionApplication()
{
}

int AdditionalActionApplication::run()
{
  CommandLine args;

  CommandLineFormat format[] = {
    { LOCK_WORKSTATION_KEY, NO_ARG },
    { LOGOUT_KEY, NO_ARG }
  };

  try {

    WinCommandLineArgs cmdArgs(m_commandLine);
    if (!args.parse(format,
                    sizeof(format) / sizeof(CommandLineFormat),
                    &cmdArgs)) {
    }
  } catch (...) {
    TvnServerHelp::showUsage();
    return 0;
  }

  try {
    if (args.optionSpecified(LOCK_WORKSTATION_KEY)) {
      Workstation::lock();
    } else if (args.optionSpecified(LOGOUT_KEY)) {
      Workstation::logOff();
    }
  } catch (SystemException &sysEx) {
    return sysEx.getErrorCode();
  }
  return 0;
}
