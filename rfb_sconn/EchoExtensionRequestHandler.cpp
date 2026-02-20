// Copyright (C) 2009,2010,2011,2012,2020 GlavSoft LLC.
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
#include "EchoExtensionRequestHandler.h"
#include "remoting/remoting_common/rfb/MsgDefs.h"
#include "remoting/remoting_common/io/DataOutputStream.h"

#include "remoting/remoting_common/io/ByteArrayOutputStream.h"
#include "remoting/remoting_common/network/RfbOutputGate.h"
#include "remoting/remoting_common/network/RfbInputGate.h"
#include "remoting/remoting_common/thread/AutoLock.h"
#include "remoting/remoting_common/server_config/Configurator.h"
#include "remoting/remoting_common/win_system/SystemException.h"
#include "remoting/remoting_common/rfb/VendorDefs.h"

EchoExtensionRequestHandler::EchoExtensionRequestHandler(RfbCodeRegistrator *registrator,
                                                       RfbOutputGate *output,
                                                       LogWriter *log,
                                                       bool enabled)
: m_output(output), m_enabled(enabled),
  m_log(log)
{

  if (!isEchoExtensionEnabled()) {
    return ;
  }

  registrator->addClToSrvCap(ClientMsgDefs::ECHO_REQUEST, VendorDefs::TIGHTVNC, EchoExtensionDefs::ECHO_REQUEST_SIG);

  registrator->addSrvToClCap(ServerMsgDefs::ECHO_RESPONSE, VendorDefs::TIGHTVNC, EchoExtensionDefs::ECHO_RESPONSE_SIG);

  registrator->regCode(ClientMsgDefs::ECHO_REQUEST, this);

  m_log->debug("Echo extension request handler created");
}

EchoExtensionRequestHandler::~EchoExtensionRequestHandler()
{
  m_log->debug("Echo extension request handler deleted");
}

void EchoExtensionRequestHandler::onRequest(unsigned int reqCode, RfbInputGate *backGate)
{
  m_input = backGate;

  try {
    if (reqCode == ClientMsgDefs::ECHO_REQUEST) {
      unsigned int number = m_input->readUInt32();
      m_log->debug("got echo request with number {}", number);
      {
        AutoLock l(m_output);

        m_output->writeUInt32(ServerMsgDefs::ECHO_RESPONSE);
        m_output->writeUInt32(number);
        m_output->flush();
      }
    }   
  } catch (::remoting::Exception &someEx) {
    m_log->error("Echo extension request failed: \"{}\"", someEx.get_message());
  } // try / catch.

  m_input = NULL;
}

bool EchoExtensionRequestHandler::isEchoExtensionEnabled()
{
  return m_enabled;
}


