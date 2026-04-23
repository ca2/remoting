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
#include "ServiceControlCommandLine.h"

const char ServiceControlCommandLine::INSTALL_SERVICE[]   = "-install";
const char ServiceControlCommandLine::REMOVE_SERVICE[]    = "-remove";
const char ServiceControlCommandLine::REINSTALL_SERVICE[] = "-reinstall";
const char ServiceControlCommandLine::START_SERVICE[]     = "-start";
const char ServiceControlCommandLine::STOP_SERVICE[]      = "-stop";

const char ServiceControlCommandLine::DONT_ELEVATE[]            = "-dontelevate";
const char ServiceControlCommandLine::SILENT[]            = "-silent";

ServiceControlCommandLine::ServiceControlCommandLine()
{
}

ServiceControlCommandLine::~ServiceControlCommandLine()
{
}

void ServiceControlCommandLine::parse(const ::subsystem::CommandLineArguments *cmdArgs)
{
   ::subsystem::CommandLineFormat format[] = {
      {INSTALL_SERVICE, ::subsystem::NO_ARG},
                                 {REMOVE_SERVICE, ::subsystem::NO_ARG},
                                 {REINSTALL_SERVICE, ::subsystem::NO_ARG},
                                 {START_SERVICE, ::subsystem::NO_ARG},
                                 {STOP_SERVICE, ::subsystem::NO_ARG},
                                 {SILENT, ::subsystem::NO_ARG},
                                 {DONT_ELEVATE, ::subsystem::NO_ARG}
  };

  if (!CommandLine::parse(format, sizeof(format) / sizeof(::subsystem::CommandLineFormat),
                          cmdArgs)) {
    throw ::subsystem::Exception("invalid command line");
  } // if cannot parse it.

  // Check additional rules.

  if (keySpecified(SILENT)) {
    if (m_foundKeys.size() != (dontElevate() ? 3 : 2)) {
      throw ::subsystem::Exception("-silent key can be used only when one command specified");
    }
  } else if (m_foundKeys.size() != (dontElevate() ? 2 : 1)) {
    throw ::subsystem::Exception("only one service command can be specified");
  }
}

bool ServiceControlCommandLine::keySpecified(const ::scoped_string & scopedstrKey) const
{
  return optionSpecified(scopedstrKey);
}

bool ServiceControlCommandLine::installationRequested() const
{
  return keySpecified(INSTALL_SERVICE);
}

bool ServiceControlCommandLine::removalRequested() const
{
  return keySpecified(REMOVE_SERVICE);
}

bool ServiceControlCommandLine::reinstallRequested() const
{
  return keySpecified(REINSTALL_SERVICE);
}

bool ServiceControlCommandLine::startRequested() const
{
  return keySpecified(START_SERVICE);
}

bool ServiceControlCommandLine::stopRequested() const
{
  return keySpecified(STOP_SERVICE);
}

bool ServiceControlCommandLine::beSilent() const
{
  return keySpecified(SILENT);
}

bool ServiceControlCommandLine::dontElevate() const
{
  return keySpecified(DONT_ELEVATE);
}
