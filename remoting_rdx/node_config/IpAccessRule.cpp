// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
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
//#include "subsystem/platform/::string.h"

namespace remoting_rfb_node
{
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
                        (::i32)m_action);
      } else {
         output.format("{}:{}", m_firstIp, (::i32)m_action);
      }
   }

   bool IpAccessRule::parse(const_char_pointer pszString, IpAccessRule *rule)
   {
      if (!parseIpRange(pszString, rule)) {
         if (!parseIp(pszString, rule))
         {
            if (!parseSubnet(pszString, rule))
            {
               return false;
            } // cannot parse string as ip/netmask
         } // cannot parse single ip
      } // cannot parse string as ip range
      return true;
   }

   bool IpAccessRule::parseIp(const_char_pointer pszString, IpAccessRule *rule)
   {
      ::i8 firstIp[16];
      ::i32 action;
      ::i8 c;

      if (sscanf(pszString, "%15[0123456789.]:%d%c", &firstIp, &action, &c) != 2) {
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

   bool IpAccessRule::parseIpRange(const_char_pointer pszString, IpAccessRule *rule)
   {
      ::i8 firstIp[16];
      ::i8 lastIp[16];
      ::i32 action;
      ::i8 c;

      if (sscanf(pszString, "%15[0123456789.]-%15[0123456789.]:%d%c", &firstIp, &lastIp, &action, &c) != 3) {
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

   bool IpAccessRule::parseSubnet(const_char_pointer pszString, IpAccessRule *rule)
   {
      ::i8 pattern[16];
      ::i8 subnet[16];
      ::i32 action;
      ::i8 c;

      if (sscanf(pszString, "%15[0123456789.]/%15[0123456789.]:%d%c", &pattern, &subnet, &action, &c) != 3) {
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
      firstIp = m_firstIp;
   }

   void IpAccessRule::getLastIp(::string & lastIp) const
   {
      lastIp = m_lastIp;
   }

   void IpAccessRule::setFirstIp(const_char_pointer pszFirstIp)
   {
      m_firstIp= pszFirstIp;
   }

   void IpAccessRule::setLastIp(const_char_pointer pszLastIp)
   {
      m_lastIp= pszLastIp;
   }

   bool IpAccessRule::operator == (const IpAccessRule & other) const
   {
      if (other.m_firstIp == m_firstIp &&
          other.m_lastIp == m_lastIp)
      {
         return true;
      }
      return false;
   }

   //
   // TODO: Create own inet_addr method that will support ::i8 strings
   //

   bool IpAccessRule::isIncludingAddress(ulong ip) const
   {
      ::string firstIpAnsi(m_firstIp);
      ::string lastIpAnsi(m_lastIp);

      ulong firstIp = MainSubsystem().internet_address4(firstIpAnsi);
      ulong lastIp  = firstIp;

      if (m_lastIp.has_character()) {
         lastIp = MainSubsystem().internet_address4(lastIpAnsi);
      }

      if (ip == firstIp || ip == lastIp) {
         return true;
      }

      if (compareIp(ip, firstIp) == 1 && compareIp(ip, lastIp) == -1) {
         return true;
      }

      return false;
   }

   bool IpAccessRule::isIpAddressStringValid(const_char_pointer pszString)
   {

      //::string ipStorage();
      ::string_array stra;

      stra.explode(".", pszString, false);
      //::string stringArray[4];
      //size_t arraySize = 4;
      //if (!ipStorage.split(".", &stringArray[0], &arraySize)) {
      //return false;
      //}
      if (stra.get_count() != 4) {
         return false;
      }
      for (size_t i = 0; i < stra.get_count(); i++) {
         if (!tryParseIPPart(stra[i])) {
            return false;
         } // if
      } // for
      return true;
   }

   ::i32 IpAccessRule::compareIp(ulong ip1, ulong ip2)
   {
      if (ip1 == ip2) {
         return 0;
      }

      ulong l1 = MainSubsystem().host_to_network_long(ip1);
      ulong l2 = MainSubsystem().host_to_network_long(ip2);

      if (l1 > l2) {
         return 1;
      }

      return -1;
   }

   bool IpAccessRule::tryParseIPPart(const_char_pointer pszString)
   {
      auto len = ::string_get_length(pszString);
      if (len < 1 || strspn(pszString, "0123456789") != len)
         return false;	// not a non-negative number
      if (len > 4) // length is more than 4 symbols
         return false;
      ::i32 value = atoi(pszString);
      if (value > 255)
         return false;	// not a number in the range 0..255
      return true;
   }

   void IpAccessRule::getIpRange(const_char_pointer pszIp, const_char_pointer pszNetmask,
                                 ::string * firstIp, ::string * lastIp) {
      ::string ipStorage(pszIp);
      ::string netmaskStorage(pszNetmask);

      firstIp->clear();
      lastIp->clear();

      ::string ipAnsi(ipStorage);
      ::string netmaskAnsi(netmaskStorage);

      ulong ipAddr = MainSubsystem().internet_address4(ipAnsi);
      ulong netmaskAddr = MainSubsystem().internet_address4(netmaskAnsi);
      ulong subnetAddr = ipAddr & netmaskAddr;
      ulong broadcastAddr = subnetAddr | (~netmaskAddr);


      *firstIp = MainSubsystem().internet_address4_as_string(subnetAddr);
      //in_addr addr = {0};

      //memcpy(&addr, &subnetAddr, sizeof(ulong));
      //::string subnetworkAnsi(inet_ntoa(addr));
      //subnetworkAnsi.toStringStorage(firstIp);

      *lastIp = MainSubsystem().internet_address4_as_string(broadcastAddr);
      //memcpy(&addr, &broadcastAddr, sizeof(ulong));
      //::string broadcastAnsi(inet_ntoa(addr));
      //broadcastAnsi.toStringStorage(lastIp);
   }
} // namespace remoting_rfb_node

