// Copyright (C) 2012 GlavSoft LLC.
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
#include <iomanip>
#include <sstream>

#include "ConnectionData.h"
//#include "remoting/remoting_common/util/::string.h"
#include "remoting/remoting_common/util/VncPassCrypt.h"
#include "remoting/remoting_common/viewer_core/VncAuthentication.h"

ConnectionData::ConnectionData()
: m_isEmpty(true),
  m_isSetPassword(false),
  m_isSetDispatchId(false),
  m_isIncoming(false)
{
}

ConnectionData::ConnectionData(const ConnectionData &connectionData)
: m_isEmpty(connectionData.m_isEmpty),
  m_isSetPassword(connectionData.m_isSetPassword),
  m_isSetDispatchId(connectionData.m_isSetDispatchId),
  m_isIncoming(connectionData.m_isIncoming)
{
  if (!connectionData.is_empty()) {
    m_hostPath.set(connectionData.m_hostPath.get());
  }
  if (m_isSetPassword) {
    m_defaultPassword = connectionData.m_defaultPassword;
  }
  if (m_isSetDispatchId) {
    m_dispatchId = connectionData.m_dispatchId;
  }
}

void ConnectionData::setIncoming(bool isIncoming)
{
  m_isIncoming = isIncoming;
}

bool ConnectionData::isIncoming() const
{
  return m_isIncoming;
}

bool ConnectionData::is_empty() const
{
  return m_isEmpty;
}

bool ConnectionData::isSetPassword() const
{
  return m_isSetPassword;
}

void ConnectionData::resetPassword()
{
  m_isSetPassword = false;
}

unsigned int ConnectionData::getDispatchId() const
{
  return (m_isSetDispatchId ? m_dispatchId : 0);
}

bool ConnectionData::isSetDispatchId() const
{
  return m_isSetDispatchId;
}

void ConnectionData::setDispatchId(unsigned int id)
{
  m_dispatchId = id;
  m_isSetDispatchId = true;
}

void ConnectionData::unsetDispatchId()
{
  m_isSetDispatchId = false;
}

void ConnectionData::setHost(const ::scoped_string & host)
{
  ::string chompedString = host;
  //TCHAR spaceChar[] = ;
  chompedString.erase_any_character_in(" \t\n\r");

  ::string ansiStr(chompedString);

  m_hostPath.set(ansiStr);
  m_isEmpty = false;
}

::string ConnectionData::getCryptedPassword() const
{
  return m_defaultPassword;
}

void ConnectionData::setCryptedPassword(const ::scoped_string & hidePassword)
{
  if (hidePassword == 0) {
    m_isSetPassword = false;
  } else {
    m_defaultPassword = hidePassword;
    m_isSetPassword = true;
  }
}

::string ConnectionData::getPlainPassword() const
{
  // Transform password from hex-string to raw data.
  ::string ansiHidePassword(m_defaultPassword);
  unsigned char encPassword[VncAuthentication::VNC_PASSWORD_SIZE];
  for (size_t i = 0; i < VncAuthentication::VNC_PASSWORD_SIZE; ++i) {
    std::stringstream passwordStream;
    passwordStream << ansiHidePassword[i * 2]
                   << ansiHidePassword[i * 2 + 1];
    int ordOfSymbol = 0;
    passwordStream >> std::hex >> ordOfSymbol;
    encPassword[i] = static_cast<unsigned char>(ordOfSymbol);
  }
  // Decrypt password.
  unsigned char plainPassword[VncAuthentication::VNC_PASSWORD_SIZE];
  VncPassCrypt::getPlainPass(plainPassword, encPassword);

  ::string ansiPlainPassword(reinterpret_cast<char *>(plainPassword));
  ::string password;
  password = ansiPlainPassword;
  return password;
}

void ConnectionData::setPlainPassword(const ::scoped_string & password)
{
  ::string ansiPlainPassword(password);
  unsigned char plainPassword[VncAuthentication::VNC_PASSWORD_SIZE];
  unsigned char encryptedPassword[VncAuthentication::VNC_PASSWORD_SIZE];
  memset(plainPassword, 0, VncAuthentication::VNC_PASSWORD_SIZE);
  memcpy(plainPassword,
         ansiPlainPassword,
         minimum(VncAuthentication::VNC_PASSWORD_SIZE, ansiPlainPassword.length()));
  VncPassCrypt::getEncryptedPass(encryptedPassword, plainPassword);
  unsigned char hidePasswordChars[VncAuthentication::VNC_PASSWORD_SIZE * 2 + 1];
  hidePasswordChars[VncAuthentication::VNC_PASSWORD_SIZE * 2] = 0;
  for (size_t i = 0; i < VncAuthentication::VNC_PASSWORD_SIZE; ++i) {
    std::stringstream passwordStream;
    int ordOfSymbol = encryptedPassword[i];
    passwordStream << std::hex << std::setw(2) << std::setfill('0') << ordOfSymbol;
    passwordStream >> hidePasswordChars[i * 2] >> hidePasswordChars[i * 2 + 1];
  }
  ::string ansiHidePassword(reinterpret_cast<char *>(hidePasswordChars));

  // save password
  m_defaultPassword = ansiHidePassword;
  m_isSetPassword = true;
}

::string ConnectionData::getHost() const
{
  ::string host;
  ::string hostAnsi(m_hostPath.get());
  host = hostAnsi;
  return host;
}

::string ConnectionData::getReducedHost() const
{
  ::string ansiStr(m_hostPath.getVncHost());
  return ansiStr;
}

int ConnectionData::getPort() const
{
  return m_hostPath.getVncPort();
}
