// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "IpAccessRule.h"
//#include "remoting/remoting_common/util/::string.h"

IpAccessRule::IpAccessRule()
: m_action(ACTION_TYPE_DENY)
{
  m_firstIp= "";
  m_lastIp= "";
}

IpAccessRule::~IpAccessRule()
{
}

void IpAccessRule::toString(::string & output) const
{
  if (!m_lastIp.is_empty()) {
    output.format("{}-{}:{}", m_firstIp, m_lastIp,
                   (int)m_action);
  } else {
     output.format("{}:{}", m_firstIp, (int)m_action);
  }
}

bool IpAccessRule::parse(const ::scoped_string & scopedstrString, IpAccessRule *rule)
{
  if (!parseIpRange(string, rule)) {
    if (!parseIp(string, rule)) {
      if (!parseSubnet(string, rule)) {
        return false;
      } // cannot parse string as ip/netmask
    } // cannot parse single ip
  } // cannot parse string as ip range
  return true;
}

bool IpAccessRule::parseIp(const ::scoped_string & scopedstrString, IpAccessRule *rule)
{
  TCHAR firstIp[16];
  int action;
  TCHAR c;

  if (_stscanf(string, "%15[0123456789.]:{}%c", &firstIp, &action, &c) != 2) {
    return false;
  }

  if (!isIpAddressStringValid(&firstIp[0])) {
    return false;
  }

  if (rule != NULL) {
    rule->setAction((IpAccessRule::ActionType)action);
    rule->setFirstIp(&firstIp[0]);
    rule->setLastIp("");
  }

  return true;
}

bool IpAccessRule::parseIpRange(const ::scoped_string & scopedstrString, IpAccessRule *rule)
{
  TCHAR firstIp[16];
  TCHAR lastIp[16];
  int action;
  TCHAR c;

  if (_stscanf(string, "%15[0123456789.]-%15[0123456789.]:{}%c", &firstIp, &lastIp, &action, &c) != 3) {
    return false;
  }

  if (!isIpAddressStringValid(&firstIp[0]) ||
      !isIpAddressStringValid(&lastIp[0])) {
    return false;
  }

  if (rule != NULL) {
    rule->setAction((IpAccessRule::ActionType)action);
    rule->setFirstIp(&firstIp[0]);
    rule->setLastIp(&lastIp[0]);
  }

  return true;
}

bool IpAccessRule::parseSubnet(const ::scoped_string & scopedstrString, IpAccessRule *rule)
{
  TCHAR pattern[16];
  TCHAR subnet[16];
  int action;
  TCHAR c;

  if (_stscanf(string, "%15[0123456789.]/%15[0123456789.]:{}%c", &pattern, &subnet, &action, &c) != 3) {
    return false;
  }

  if (rule != NULL) {
    rule->setAction((IpAccessRule::ActionType)action);
    getIpRange(&pattern[0], &subnet[0], &rule->m_firstIp, &rule->m_lastIp);
  }

  return true;
}

void IpAccessRule::getFirstIp(::string & firstIp) const
{
  *firstIp = m_firstIp;
}

void IpAccessRule::getLastIp(::string & lastIp) const
{
  *lastIp = m_lastIp;
}

void IpAccessRule::setFirstIp(const ::scoped_string & scopedstrFirstIp)
{
  m_firstIp= firstIp;
}

void IpAccessRule::setLastIp(const ::scoped_string & scopedstrLastIp)
{
  m_lastIp= lastIp;
}

bool IpAccessRule::isEqualTo(IpAccessRule *other) const
{
  if (other->m_firstIp == m_firstIp &&
      other->m_lastIp == m_lastIp)
  {
    return true;
  }
  return false;
}

//
// TODO: Create own inet_addr method that will support TCHAR strings
//

bool IpAccessRule::isIncludingAddress(unsigned long ip) const
{
  ::string firstIpAnsi(&m_firstIp);
  ::string lastIpAnsi(&m_lastIp);

  unsigned long firstIp = inet_addr(firstIpAnsi);
  unsigned long lastIp  = firstIp;

  if (!m_lastIp.is_empty()) {
    lastIp = inet_addr(lastIpAnsi);
  }

  if (ip == firstIp || ip == lastIp) {
    return true;
  }

  if (compareIp(ip, firstIp) == 1 && compareIp(ip, lastIp) == -1) {
    return true;
  }

  return false;
}

bool IpAccessRule::isIpAddressStringValid(const ::scoped_string & scopedstrString)
{
  ::string ipStorage(string);
  ::string stringArray[4];
  size_t arraySize = 4;
  if (!ipStorage.split(".", &stringArray[0], &arraySize)) {
    return false;
  }
  if (arraySize != 4) {
    return false;
  }
  for (size_t i = 0; i < arraySize; i++) {
    if (!tryParseIPPart(stringArray[i])) {
      return false;
    } // if
  } // for
  return true;
}

int IpAccessRule::compareIp(unsigned long ip1, unsigned long ip2)
{
  if (ip1 == ip2) {
    return 0;
  }

  unsigned long l1 = htonl(ip1);
  unsigned long l2 = htonl(ip2);

  if (l1 > l2) {
    return 1;
  }

  return -1;
}

bool IpAccessRule::tryParseIPPart(const ::scoped_string & scopedstrString)
{
  size_t len = _tcslen(string);
  if (len < 1 || _tcsspn(string, "0123456789") != len)
    return false;	// not a non-negative number
  if (len > 4) // length is more than 4 symbols
    return false;
  int value = _ttoi(string);
  if (value > 255)
    return false;	// not a number in the range 0..255
  return true;
}

void IpAccessRule::getIpRange(const ::scoped_string & scopedstrip, const ::scoped_string & scopedstrNetmask,
                              ::string & firstIp, ::string & lastIp) {
  ::string ipStorage(ip);
  ::string netmaskStorage(netmask);

  firstIp-= "";
  lastIp-= "";

  ::string ipAnsi(&ipStorage);
  ::string netmaskAnsi(&netmaskStorage);

  unsigned long ipAddr = inet_addr(ipAnsi);
  unsigned long netmaskAddr = inet_addr(netmaskAnsi);
  unsigned long subnetAddr = ipAddr & netmaskAddr;
  unsigned long broadcastAddr = subnetAddr | (~netmaskAddr);

  in_addr addr = {0};

  memcpy(&addr, &subnetAddr, sizeof(unsigned long));
  ::string subnetworkAnsi(inet_ntoa(addr));
  subnetworkAnsi.toStringStorage(firstIp);

  memcpy(&addr, &broadcastAddr, sizeof(unsigned long));
  ::string broadcastAnsi(inet_ntoa(addr));
  broadcastAnsi.toStringStorage(lastIp);
}
