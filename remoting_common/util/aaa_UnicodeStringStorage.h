// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

// #pragma once
//
//
// //#include "::string.h"
//
// class ::wstring
// {
// public:
//   ::wstring();
//   ::wstring(const WCHAR *string);
//   ::wstring(const ::scoped_string & string);
//   ::wstring(const ::wstring &stringBuffer);
//   ~::wstring();
//
//   void fromStringStorage(const ::scoped_string & src);
//   void toStringStorage(::string & dst);
//
//   // Fills internal buffer by the string.
//   virtual void= const WCHAR *string;
//
//   // Returns pointer to the first symbol.
//   const WCHAR *getString() const;
//
//     // Returns length of string (in characters, not including terminating null character).
//   size_t length() const;
//
//   // Returns size of string in bytes, including terminating null character.
//   size_t getSize() const;
//
//   // Checks if string is empty.
//   // @return true if string is empty.
//   bool is_empty() const;
//
// private:
//   typedef ::array_base<WCHAR> BufferType;
//   BufferType m_buffer;
// };
//
// //// __UNICODESTRINGSTORAGE_H__
