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
#include "PortMappingRect.h"
// #include aaa_<tchar.h>
// #include aaa_<stdio.h>

namespace remoting_node
{
   PortMappingRect::PortMappingRect(int l, int t, int r, int b)
    : m_rectangle(l, t, r, b)
   {
   }

   PortMappingRect::PortMappingRect()
   {
   }

   PortMappingRect::~PortMappingRect()
   {
   }

   void PortMappingRect::toString(::string & string) const
   {
      string.format("%dx{}+{}+{}", m_rectangle.width(), m_rectangle.height(), m_rectangle.left, m_rectangle.top);
   }

   bool PortMappingRect::parse(const ::scoped_string & scopedstr, PortMappingRect * pportmappingrectOut)
   {

      ::string str(scopedstr);
      int width, height, x, y;
      char c;
      if (sscanf(str.c_str(), "%dx{}+{}+{}%c", &width, &height, &x, &y, &c) != 4) {
         return false;
      }
      if (width < 0 || height < 0) {
         return false;
      }
      if (pportmappingrectOut != NULL) {
         pportmappingrectOut->m_rectangle.left = x;
         pportmappingrectOut->m_rectangle.right = x + width;
         pportmappingrectOut->m_rectangle.top = y;
         pportmappingrectOut->m_rectangle.bottom = y + height;
      }
      return true;
   }

   bool PortMappingRect::tryParse(const char * psz)
   {
      return parse(psz, NULL);
   }
} // namespace remoting_node

