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
#include "ServerCommandLine.h"
#include "subsystem/CommandLine.h"

ServerCommandLine::ServerCommandLine()
{
}

ServerCommandLine::~ServerCommandLine()
{
}

bool ServerCommandLine::parse(const ::subsystem::CommandLineArguments *cmdArgs)
{
  ::subsystem::CommandLineFormat format[] = {{"-help", ::subsystem::NO_ARG},
                                              {"-h", ::subsystem::NO_ARG},
                                              {"-?", ::subsystem::NO_ARG},
                                              {"/help", ::subsystem::NO_ARG},
                                              {"/h", ::subsystem::NO_ARG},
                                              {"/?", ::subsystem::NO_ARG},
                                              {"-run", ::subsystem::NO_ARG}
  };

  if (!CommandLine::parse(format, sizeof(format) / sizeof(::subsystem::CommandLineFormat), cmdArgs))
  {
    return false;
  }
  if (showHelp() && optionSpecified("-run")) {
    return false;
  }

  return true;
}

bool ServerCommandLine::showHelp()
{
  return optionSpecified("-help") || optionSpecified("-h") ||
         optionSpecified("-?") || optionSpecified("/help") ||
         optionSpecified("/h") || optionSpecified("/?");
}
