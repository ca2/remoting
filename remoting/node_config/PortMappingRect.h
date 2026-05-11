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

#pragma once


#include "subsystem/_common_header.h"
//////#include "subsystem/platform/::string.h"
#include "acme/prototype/geometry2d/rectangle.h"
namespace remoting_node
{
   class CLASS_DECL_REMOTING PortMappingRect
   {
   public:

      ::i32_rectangle m_rectangle;


      PortMappingRect(int left, int right, int top, int bottom);
      PortMappingRect();
      ~PortMappingRect();

      // Converts this rectangle to string value.
      void toString(::string & string) const;

      ::string toString() const
      {
         ::string str;
         toString(str);
         return str;

      }

      // Returns true if string can be parsed to rectangle object,
      // false otherwise.
      static bool tryParse(const char * psz);

      // Parsed string and sets parsed values to output rectangle.
      // Returns true if string is valid, false otherwise.
      // Remark: "out" parameter can be null.
      static bool parse(const ::scoped_string & scopedstr,  PortMappingRect * pportmappingrectOut);


      bool parse(const ::scoped_string & scopedstr)
      {

         return PortMappingRect::parse(scopedstr, this);

      }


      bool operator == (const PortMappingRect & portmappingrect) const
      {

         return m_rectangle == portmappingrect.m_rectangle;

      }


   };
} // namespace remoting_node

