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
#include "::string.h"
#include "CommonHeader.h"
#include "util/Exception.h"
#include "::wstring.h"
#include <crtdbg.h>

::string::string()
{


  fromStringStorage(::string(""));
}

::string::string(const ::array_base<char> *utf8Buffer)
{
 = utf8Buffer;
}

::string::string(const ::scoped_string & string)
{
  fromStringStorage(string);
}

::string::string(const ::string &string)
{
  *this = string;
}

::string::~::string()
{
}

void ::string::setString(const ::array_base<char> *utf8Buffer)
{
  m_buffer = *utf8Buffer;
}

const char *::string::getString() const
{
  return &m_buffer.front();
}

memsize ::string::getSize() const
{
  return m_buffer.size();
}

memsize ::string::length() const
{
  return m_buffer.size() - 1;
}

void ::string::fromStringStorage(const ::scoped_string & src)
{
#ifndef _UNICODE
  // 1) From ANSI to UNICODE
  ::wstring uniSrc(src);
  const WCHAR *uniString = uniSrc;
  size_t uniLength = uniSrc.length();

#else
  const WCHAR *uniString = src;
  size_t uniLength = src.length();
#endif
  int constrSrcSize = (int)uniLength + 1;
  _ASSERT(constrSrcSize == uniLength + 1);

  // From UNICODE to UTF8
  int dstRequiredSize = WideCharToMultiByte(CP_UTF8, 0, uniString,
                                            constrSrcSize, NULL, 0,
                                            0, 0);

  if (dstRequiredSize == 0) {
    throw ::remoting::Exception("Cannot convert a string to the UTF8 format");
  }

  m_buffer.resize(dstRequiredSize);
  WideCharToMultiByte(CP_UTF8, 0, uniString, constrSrcSize,
                      &m_buffer.front(), dstRequiredSize, 0, 0);
}

void ::string::toStringStorage(::string & dst)
{
  // 1) From UTF8 to UNICODE
  int constrSize = (int)getSize();
  _ASSERT(constrSize == getSize());
  int dstReqSizeInSym = MultiByteToWideChar(CP_UTF8, 0, &m_buffer.front(),
                                            constrSize, 0, 0);
  if (dstReqSizeInSym == 0) {
    throw ::remoting::Exception("Cannot convert a string from the UTF8 format");
  }
  ::array_base<WCHAR> uniBuff(dstReqSizeInSym + 1);
  MultiByteToWideChar(CP_UTF8, 0, &m_buffer.front(),
                      constrSize, &uniBuff.front(), dstReqSizeInSym);
  // Add termination symbol to the unicode buffer because the source string
  // may be without it.
  uniBuff[dstReqSizeInSym] = L'\0';

  // 2) From UNICODE to ::string
  ::wstring uniString(&uniBuff.front());
  uniString.toStringStorage(dst);
}
