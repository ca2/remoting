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
#include "ControlMessage.h"
#include "ControlAuth.h"
#include "ControlAuthDialog.h"
#include "ControlProto.h"
#include "remoting/remoting_common/server_config/Configurator.h"
#include "remoting/remoting_common/config/RegistrySettingsManager.h"
#include "remoting/remoting_common/util/VncPassCrypt.h"
//#include "remoting/remoting_common/util/::string.h"
#include "remoting_node_desktop/NamingDefs.h"
//#include "file_lib/WinFile.h"

#include "remoting_node/resource.h"

#include <crtdbg.h>

ControlMessage::ControlMessage(unsigned int messageId, ControlGate *gate,
                               const ::scoped_string & scopedstrPasswordFile,
                               bool getPassFromConfigEnabled,
                               bool forService)
: DataOutputStream(0), m_messageId(messageId), m_gate(gate),
  m_passwordFile(passwordFile),
  m_getPassFromConfigEnabled(getPassFromConfigEnabled),
  m_forService(forService)
{
  m_tunnel = new ByteArrayOutputStream(2048);

  m_outStream = m_tunnel;
}

ControlMessage::~ControlMessage()
{
  delete m_tunnel;
}

void ControlMessage::send()
{
  sendData();

  checkRetCode();
}

void ControlMessage::sendData()
{
  m_gate->writeUInt32(m_messageId);
  _ASSERT((unsigned int)m_tunnel->size() == m_tunnel->size());
  m_gate->writeUInt32((unsigned int)m_tunnel->size());
  m_gate->writeFully(m_tunnel->toByteArray(), m_tunnel->size());
}

void ControlMessage::checkRetCode()
{
  unsigned int messageId = m_gate->readUInt32();

  switch (messageId) {
  case ControlProto::REPLY_ERROR:
    {
      ::string scopedstrMessage;
      m_gate->readUTF8(&scopedstrMessage);
      throw RemoteException(scopedstrMessage);
    }
    break;
  case ControlProto::REPLY_AUTH_NEEDED:
    if (m_passwordFile.length() != 0) {
      authFromFile();
    } else if (m_getPassFromConfigEnabled) {
      authFromRegistry();
    } else {
      ControlAuthDialog authDialog;

      int retCode = authDialog.showModal();
      switch (retCode) {
      case IDCANCEL:
        throw ControlAuthException(StringTable::getString(IDS_USER_CANCEL_CONTROL_AUTH), true);
      case IDOK:
        ControlAuth auth(m_gate, authDialog.getPassword());
        send();
        break;
      }
    }
    break;
  case ControlProto::REPLY_OK:
    break;
  default:
    _ASSERT(FALSE);
    throw RemoteException("Unknown ret code.");
  }
}

void ControlMessage::authFromFile()
{
  WinFile file(m_passwordFile, F_READ, FM_OPEN);
  char ansiBuff[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  file.read(ansiBuff, 8);
  for (int i = 0; i < 8; i++) {
    if (ansiBuff[i] == '\r' || ansiBuff[i] == '\n') {
      ansiBuff[i] = '\0';
    }
  }
  ::string ansiPwd(ansiBuff);
  ::string password;
  ansiPwd.toStringStorage(&password);
  ControlAuth auth(m_gate, password);
  send();
}

void ControlMessage::authFromRegistry()
{
  HKEY rootKey = m_forService ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
  RegistrySettingsManager sm(rootKey, RegistryPaths::SERVER_PATH, 0);

  unsigned char hidePassword[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  unsigned char plainPassword[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  size_t passSize = sizeof(hidePassword);

  if (sm.getBinaryData("ControlPassword",
                       hidePassword,
                       &passSize)) {
    VncPassCrypt::getPlainPass(plainPassword, hidePassword);

    ::string plainAnsiString((char *)plainPassword);
    ::string password;
    plainAnsiString.toStringStorage(&password);
    // Clear ansi plain password from memory.
    memset(plainPassword, 0, sizeof(plainPassword));
    ControlAuth auth(m_gate, password);

    send();
  } else {
    // Ignore errors for silent.
  }
}
