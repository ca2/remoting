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
#include "PortMapping.h"
#include "subsystem/platform/StringParser.h"

// #include aaa_<tchar.h>
// #include aaa_<stdio.h>

namespace remoting_node
{
   PortMapping::PortMapping()
   : m_port(0)
   {
   }

   PortMapping::PortMapping(int nport, PortMappingRect nrect)
   : m_port(nport), m_rect(nrect)
   {
   }

   PortMapping::PortMapping(const PortMapping &other)
   : m_port(other.m_port), m_rect(other.m_rect)
   {
   }

   PortMapping::~PortMapping()
   {
   }

   PortMapping &PortMapping::operator=(const PortMapping &other)
   {
      m_port = other.m_port;
      m_rect = other.m_rect;
      return *this;
   }

   bool PortMapping::isEqualTo(const PortMapping *other) const
   {
      return other->m_port == m_port && other->m_rect.m_rectangle == m_rect.m_rectangle;
   }

   void PortMapping::setPort(int nport)
   {
      m_port = nport;
   }

   void PortMapping::setRect(PortMappingRect nrect)
   {
      m_rect = nrect;
   }

   int PortMapping::getPort() const
   {
      return m_port;
   }

   PortMappingRect PortMapping::getRect() const
   {
      return m_rect;
   }

   void PortMapping::toString(::string & string) const
   {
      //
      // Format:   [port]:[rectangle.toString()]
      // It means: [port]:[width]x[height]+[x]+[y]
      // without square brackets.
      //

      ::string rectString;
      m_rect.toString(rectString);

      string.format("{}:{}", m_port, rectString);
   }

   ::string PortMapping::toString() const
   {

      ::string str;

      toString(str);

      return str;

   }

   bool PortMapping::parse(const char * psz, PortMapping *mapping)
   {
      int port;
      char c;
      PortMappingRect portmappingrect;
      auto pszRectString = strchr(psz, ':') + 1;
      if (pszRectString == NULL)
      {
         return false;
      }
      if ((sscanf(psz, "%d%c", &port, &c) != 2) || (c != ':')) {
         return false;
      }
      if (port < 0) {
         return false;
      }
      if (!PortMappingRect::parse(pszRectString, &portmappingrect))
      {
         return false;
      }
      if (mapping != NULL) {
         mapping->setPort(port);
         mapping->setRect(portmappingrect);
      }
      return true;
   }
} // namespace remoting_node

