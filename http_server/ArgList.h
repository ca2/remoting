// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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


//#include aaa_<map>
//#include aaa_<string>



class CLASS_DECL_REMOTING ArgList
{
public:
  ArgList(const_char_pointer argString);
  virtual ~ArgList();

  // Return arguments count.
  size_t getCount() const;

  // Returns key name with specified index or NULL if index out of range.
  // It can be used for enumerating keys.
  const_char_pointer getKey(size_t index);

  // Returns value of argument with specified key or NULL if no such
  // argument.
  const_char_pointer getValue(const_char_pointer key);

protected:
  void splitPair(const_char_pointer par, ::i8 delimitter, char_pointer key, char_pointer value);

  void htmlDecode(char_pointer value) const;

protected:
  ::map<string, char_pointer > m_args;
};


