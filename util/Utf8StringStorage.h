//// Copyright (C) 2010,2011,2012 GlavSoft LLC.
//// All rights reserved.
////
////-------------------------------------------------------------------------
//// This file is part of the TightVNC software.  Please visit our Web site:
////
////                       http://www.tightvnc.com/
////
//// This program is free software; you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation; either version 2 of the License, or
//// (at your option) any later version.
////
//// This program is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License along
//// with this program; if not, write to the Free Software Foundation, Inc.,
//// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////-------------------------------------------------------------------------
////
//#pragma once
//
//class ::string;
//
////#include <vector>
//
//// Note: The Utf8StringStorage class contains utf8 symbols "as is". The class
//// do not adds termination symbol(s) and do not remove it when
//// it already presented.
//class Utf8StringStorage
//{
//private:
//
//   ::array_base<char> m_buffer;
//
//public:
//  Utf8StringStorage();
//  Utf8StringStorage(const ::array_base<char> *utf8Buffer);
//  Utf8StringStorage(const ::scoped_string & string);
//  Utf8StringStorage(const Utf8StringStorage &string);
//  ~Utf8StringStorage();
//
//  // Sets internal buffer by an external utf8 string.
//  void= const ::array_base<char> *utf8Buffer;
//
//  // Returns pointer to the string.
//  const char *getString() const;
//
//  // Returns size of string in bytes, including terminating null character.
//  memsize getSize() const;
//  // Returns length of string (in characters, not including terminating null character).
//  memsize getLength() const;
//  void fromStringStorage(const ::scoped_string & src);
//  void toStringStorage(::string & dst);
//
//};
//
//
//
//
