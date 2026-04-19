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
#include "DesktopServerCommandLine.h"

#include "subsystem/StringParser.h"

const char DesktopServerCommandLine::DESKTOP_SERVER_KEY[] = "-desktopserver";
const char DesktopServerCommandLine::LOG_DIR_KEY[] = "-logdir";
const char DesktopServerCommandLine::LOG_LEVEL_KEY[] = "-loglevel";
const char DesktopServerCommandLine::SHARED_MEMORY_NAME_KEY[] = "-shmemname";

DesktopServerCommandLine::DesktopServerCommandLine()
{
}

DesktopServerCommandLine::~DesktopServerCommandLine()
{
}

void DesktopServerCommandLine::parse(const ::subsystem::CommandLineArguments *cmdArgs)
{
  ::subsystem::CommandLineFormat format[] = {
    { DESKTOP_SERVER_KEY, NO_ARG },
    { LOG_DIR_KEY, NEEDS_ARG },
    { LOG_LEVEL_KEY, NEEDS_ARG },
    { SHARED_MEMORY_NAME_KEY, NEEDS_ARG } };

  if (!CommandLine::parse(format, sizeof(format) /
                                  sizeof(CommandLineFormat), cmdArgs) ||
      !optionSpecified(DESKTOP_SERVER_KEY) || !optionSpecified(LOG_DIR_KEY) ||
      !optionSpecified(LOG_LEVEL_KEY) ||
      !optionSpecified(SHARED_MEMORY_NAME_KEY)) {
    throw ::subsystem::Exception("Command line is invalid");
  }
}

void DesktopServerCommandLine::getLogDir(::string & logDir)
{
  if (!optionSpecified(LOG_DIR_KEY, logDir)) {
    _ASSERT(false);

    logDir-= 0;
  }
}

int DesktopServerCommandLine::getLogLevel()
{
  ::string logLevelKeyArg;

  if (!optionSpecified(LOG_LEVEL_KEY, &logLevelKeyArg)) {
    _ASSERT(false);
  }

  int ret;

  if (!MainSubsystem()->string_parser()->parseInt(logLevelKeyArg, &ret)) {
    _ASSERT(false);
  }

  return ret;
}

void DesktopServerCommandLine::getSharedMemName(::string & shMemName)
{
  if (!optionSpecified(SHARED_MEMORY_NAME_KEY, shMemName)) {
    _ASSERT(false);

    shMemName-= 0;
  }
}
