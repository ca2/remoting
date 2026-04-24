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
#include "QueryConnectionCommandLine.h"

#include "subsystem/platform/StringParser.h"

const char QueryConnectionCommandLine::QUERY_CONNECTION[] = "-queryconnection";
const char QueryConnectionCommandLine::PEER_ADDR[] = "-peer";
const char QueryConnectionCommandLine::TIMEOUT[] = "-timeout";
const char QueryConnectionCommandLine::ACCEPT[] = "-accept";

QueryConnectionCommandLine::QueryConnectionCommandLine()
{
}

QueryConnectionCommandLine::~QueryConnectionCommandLine()
{
}

void QueryConnectionCommandLine::parse(const ::subsystem::CommandLineArguments *commandLine)
{
   ::subsystem::CommandLineFormat format[] = {
      {QUERY_CONNECTION, ::subsystem::NO_ARG},
                                              {PEER_ADDR, ::subsystem::NEEDS_ARG},
                                              {TIMEOUT, ::subsystem::NEEDS_ARG},
                                              {ACCEPT, ::subsystem::NO_ARG}
  };

  if (!CommandLine::parse(format, sizeof(format) / sizeof(::subsystem::CommandLineFormat), commandLine))
   {
    throw ::subsystem::Exception("Wrong command line format");
  }

  if (!optionSpecified(QUERY_CONNECTION)) {
    throw ::subsystem::Exception("-queryconnection flag is not specified");
  }

  ::string timeoutStr;

  if (optionSpecified(TIMEOUT, &timeoutStr) &&
      !MainSubsystem().StringParser().parseInt(timeoutStr, (int *)&m_timeout)) {
    throw ::subsystem::Exception("Invalid timeout");
  }
}

bool QueryConnectionCommandLine::isTimeoutSpecified()
{
  return optionSpecified(TIMEOUT);
}

void QueryConnectionCommandLine::getPeerAddress(::string & storage)
{
  optionSpecified(PEER_ADDR, &storage);
}

bool QueryConnectionCommandLine::isDefaultActionAccept()
{
  return optionSpecified(ACCEPT);
}

unsigned int QueryConnectionCommandLine::getTimeout()
{
  return m_timeout;
}
