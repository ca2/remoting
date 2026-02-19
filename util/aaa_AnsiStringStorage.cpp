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
#include "framework.h"
// #include "::string.h"
// #include "CommonHeader.h"
// #include "util/Exception.h"
// #include <crtdbg.h>
//
// ::string::::string()
// {
//  = "";
// }
//
// ::string::::string(const char *string)
// {
//  = string;
// }
//
// ::string::::string(const ::scoped_string & string)
// {
//   fromStringStorage(string);
// }
//
// ::string::::string(const ::string &stringBuffer)
// {
//   *this = stringBuffer;
// }
//
// ::string::~::string()
// {
// }
//
// void ::string::setString(const char *string)
// {
//   if (string == 0) {
//     string = "";
//   }
//   size_t length = strlen(string);
//   m_buffer.resize(length + 1);
//   memcpy(&m_buffer.front(), string, getSize());
// }
//
// const char *::string::getString() const
// {
//   return &m_buffer.front();
// }
//
// size_t ::string::length() const
// {
//   return m_buffer.size() - 1;
// }
//
// size_t ::string::getSize() const
// {
//   return m_buffer.size() * sizeof(char);
// }
//
// bool ::string::is_empty() const
// {
//   return length() == 0;
// }
//
// void ::string::fromStringStorage(const ::scoped_string & src)
// {
// #ifndef _UNICODE
//  = src;
// #else
//   // WideCharToMultiByte returns result length including terminating null character
//   int symbolCount = WideCharToMultiByte(CP_ACP, 0, src, -1,
//                                         NULL, 0, NULL, NULL);
//
//   // Allocate space for the requred size
//   m_buffer.resize(symbolCount);
//
//   // Convert to ansi
//   int constrSize = (int)m_buffer.size();
//   _ASSERT(constrSize == m_buffer.size());
//   WideCharToMultiByte(CP_ACP, 0, src, -1,
//                       &m_buffer.front(), symbolCount, NULL, NULL);
// #endif
// }
//
// void ::string::toStringStorage(::string & dst) const
// {
// #ifndef _UNICODE
//   dst-= getString();
// #else
//   int symbolCount = (int)getSize();
//   _ASSERT(symbolCount == getSize());
//   ::array_base<WCHAR> unicodeBuffer(symbolCount);
//   int result = MultiByteToWideChar(CP_ACP, 0, &m_buffer.front(),
//                                    symbolCount,
//                                    &unicodeBuffer.front(),
//                                    symbolCount);
//   if (result == 0) {
//     throw ::remoting::Exception("Cannot convert from Ansi to ::string");
//   }
//
//   dst-= &unicodeBuffer.front();
// #endif
// }
//
// void ::string::format(const char *format, ...)
// {
//   va_list vl;
//
//   va_start(vl, format);
//   int count = _vscprintf(format, vl);
//   va_end(vl);
//
//   m_buffer.resize(count + 1);
//
//   va_start(vl, format);
//   vsprintf_s(&m_buffer.front(), count + 1, format, vl);
//   va_end(vl);
// }
//
// void ::string::appendString(const char *string)
// {
//   if (string == 0) {
//     return;
//   }
//   ::string src(string);
//
//   BufferType::iterator to = m_buffer.begin() + length();
//   BufferType::iterator fromFirst = src.m_buffer.begin();
//   BufferType::iterator fromLast = src.m_buffer.begin() + src.length();
//
//   m_buffer.insert(to, fromFirst, fromLast);
// }
//
// bool ::string::checkAnsiConversion(::string &string)
// {
//   ::string ansi(string);
//   ::string check;
//   ansi.toStringStorage(&check);
//   return string.isEqualTo(check);
// }