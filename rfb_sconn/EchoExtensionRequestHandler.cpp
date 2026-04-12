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
#include "subsystem_acme/io/DataOutputStream.h"

#include "subsystem_acme/io/ByteArrayOutputStream.h"
#include "remoting/remoting_common/network/RfbOutputGate.h"
#include "remoting/remoting_common/network/RfbInputGate.h"
//#include "subsystem_acme/thread/critical_section.h"
#include "remoting/remoting_common/server_config/Configurator.h"
#include "subsystem_acme/node/SystemException.h"
#include "remoting/remoting_common/rfb/VendorDefs.h"

EchoExtensionRequestHandler::EchoExtensionRequestHandler(RfbCodeRegistrator *registrator,
                                                       RfbOutputGate *output,
                                                       ::subsystem::LogWriter *log,
                                                       bool enabled)
: m_output(output), m_enabled(enabled),
  m_plogwriter(log)
{

  if (!isEchoExtensionEnabled()) {
    return ;
  }

  registrator->addClToSrvCap(ClientMsgDefs::ECHO_REQUEST, VendorDefs::TIGHTVNC, EchoExtensionDefs::ECHO_REQUEST_SIG);

  registrator->addSrvToClCap(ServerMsgDefs::ECHO_RESPONSE, VendorDefs::TIGHTVNC, EchoExtensionDefs::ECHO_RESPONSE_SIG);

  registrator->regCode(ClientMsgDefs::ECHO_REQUEST, this);

  m_plogwriter->debug("Echo extension request handler created");
}

EchoExtensionRequestHandler::~EchoExtensionRequestHandler()
{
  m_plogwriter->debug("Echo extension request handler deleted");
}

void EchoExtensionRequestHandler::onRequest(unsigned int reqCode, RfbInputGate *backGate)
{
  m_input = backGate;

  try {
    if (reqCode == ClientMsgDefs::ECHO_REQUEST) {
      unsigned int number = m_input->readUInt32();
      m_plogwriter->debug("got echo request with number {}", number);
      {
        critical_section_lock l(m_output);

        m_output->writeUInt32(ServerMsgDefs::ECHO_RESPONSE);
        m_output->writeUInt32(number);
        m_output->flush();
      }
    }   
  } catch (::subsystem::Exception &someEx) {
    m_plogwriter->error("Echo extension request failed: \"{}\"", someEx.get_message());
  } // try / catch.

  m_input = NULL;
}

bool EchoExtensionRequestHandler::isEchoExtensionEnabled()
{
  return m_enabled;
}


