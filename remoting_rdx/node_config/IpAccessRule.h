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

#pragma once


//////#include "subsystem/platform/::string.h"

namespace remoting_rfb_node
{
   //
   // Class contains information about
   // host access (allow, deny hosts etc).
   //

   class CLASS_DECL_REMOTING_RDX IpAccessRule
   {
   public:

      //
      // Host access type
      //

      enum ActionType {
         ACTION_TYPE_ALLOW = 0,
         ACTION_TYPE_DENY  = 1,
         ACTION_TYPE_QUERY = 2
       };

   public:
      IpAccessRule();
      virtual ~IpAccessRule();
   public:

      void toString(::string & output) const;

      static bool parse(const_char_pointer pszString, IpAccessRule *rule);
      static bool parseIp(const_char_pointer pszString, IpAccessRule *rule);
      static bool parseIpRange(const_char_pointer pszString, IpAccessRule *rule);
      static bool parseSubnet(const_char_pointer pszString, IpAccessRule *rule);

      //
      // Method to access protected members
      //

      //
      // m_action
      //

      ActionType getAction() const {
         return m_action;
      }

      void setAction(ActionType value) {
         m_action = value;
      }

      //
      // Ip range
      //

      void getFirstIp(::string & firstIp) const;
      void getLastIp(::string & lastIp) const;

      void setFirstIp(const_char_pointer pszFirstIp);
      void setLastIp(const_char_pointer pszLastIp);

      //
      // Helper methods
      //

      bool operator ==(const IpAccessRule & other) const;

      //
      // Determinates belong address to subnetwork which associated
      // with this IpAccessRule instanse or not.
      //

      bool isIncludingAddress(ulong ip) const;
      static bool isIpAddressStringValid(const_char_pointer pszString);

      //
      // Return values:
      // 0  - equal
      // -1 - ip1 < ip2
      // 1  - ip1 > ip2
      //

      static ::i32 compareIp(ulong ip1, ulong ip2);

   protected:
      static bool tryParseIPPart(const_char_pointer pszString);
      static void getIpRange(const_char_pointer pszip, const_char_pointer pszNetmask,
                             ::string * firstIp, ::string * lastIp);
   protected:
      ActionType m_action;

      //
      // Ip range
      //

      ::string m_firstIp;
      ::string m_lastIp;
   };
} // namespace remoting_rfb_node

