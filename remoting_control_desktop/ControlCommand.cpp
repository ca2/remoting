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
#include "ControlCommand.h"
#include "RemoteException.h"
#include "ControlAuthException.h"

#include "remoting/remoting_common/util/CommonHeader.h"
#include "remoting/remoting_common/util/Exception.h"

#include "acme/exception/io.h"

ControlCommand::ControlCommand(Command *command, Notificator *notificator)
: m_command(command), m_notificator(notificator), m_successfull(false)
{
}

ControlCommand::~ControlCommand()
{
}

void ControlCommand::execute()
{
  _ASSERT(m_command != 0);

  m_successfull = true;

  if (m_command != NULL) {
    try {
      try {
        m_command->execute();
      } catch (::io_exception &) {
        if (m_notificator != 0) {
          m_notificator->notifyConnectionLost();
        }
        throw;
      } catch (ControlAuthException &authEx) {
        if ((m_notificator != 0) && !authEx.isSilent()) {
          m_notificator->notifyServerSideException(authEx.get_message());
        }
        throw;
      } catch (RemoteException &someEx) {
        if (m_notificator != 0) {
          m_notificator->notifyServerSideException(someEx.get_message());
        }
        throw;
      } catch (::remoting::Exception &) {
        _ASSERT(FALSE);
        throw;
      }
    } catch (...) {
      m_successfull = false;
    }
  }
}

bool ControlCommand::executionResultOk() const
{
  return m_successfull;
}
