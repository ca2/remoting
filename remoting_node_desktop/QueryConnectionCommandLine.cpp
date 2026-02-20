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

#include "QueryConnectionCommandLine.h"

#include "remoting/remoting_common/util/StringParser.h"

const TCHAR QueryConnectionCommandLine::QUERY_CONNECTION[] = "-queryconnection";
const TCHAR QueryConnectionCommandLine::PEER_ADDR[] = "-peer";
const TCHAR QueryConnectionCommandLine::TIMEOUT[] = "-timeout";
const TCHAR QueryConnectionCommandLine::ACCEPT[] = "-accept";

QueryConnectionCommandLine::QueryConnectionCommandLine()
{
}

QueryConnectionCommandLine::~QueryConnectionCommandLine()
{
}

void QueryConnectionCommandLine::parse(const CommandLineArgs *commandLine)
{
  CommandLineFormat format[] = {
    { QUERY_CONNECTION, NO_ARG },
    { PEER_ADDR, NEEDS_ARG},
    { TIMEOUT, NEEDS_ARG },
    { ACCEPT, NO_ARG }
  };

  if (!CommandLine::parse(format, sizeof(format) / sizeof(CommandLineFormat), commandLine)) {
    throw ::remoting::Exception("Wrong command line format");
  }

  if (!optionSpecified(QUERY_CONNECTION)) {
    throw ::remoting::Exception("-queryconnection flag is not specified");
  }

  ::string timeoutStr;

  if (optionSpecified(TIMEOUT, &timeoutStr) &&
      !StringParser::parseInt(timeoutStr, (int *)&m_timeout)) {
    throw ::remoting::Exception("Invalid timeout");
  }
}

bool QueryConnectionCommandLine::isTimeoutSpecified()
{
  return optionSpecified(TIMEOUT);
}

void QueryConnectionCommandLine::getPeerAddress(::string & storage)
{
  optionSpecified(PEER_ADDR, storage);
}

bool QueryConnectionCommandLine::isDefaultActionAccept()
{
  return optionSpecified(ACCEPT);
}

DWORD QueryConnectionCommandLine::getTimeout()
{
  return m_timeout;
}
