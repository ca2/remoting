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
#include "::wstring.h"
#include "CommonHeader.h"
#include "remoting/remoting_common/util/Exception.h"
#include <crtdbg.h>

::wstring::::wstring()
{
 = L"";
}

::wstring::::wstring(const WCHAR *string)
{
 = string;
}

::wstring::::wstring(const ::scoped_string & string)
{
  fromStringStorage(string);
}

::wstring::::wstring(const ::wstring &stringBuffer)
{
  *this = stringBuffer;
}

::wstring::~::wstring()
{
}

void ::wstring::setString(const WCHAR *string)
{
  if (string == 0) {
    string = L"";
  }

  size_t length = wcslen(string);
  m_buffer.resize(length + 1);
  memcpy(&m_buffer.front(), string, getSize());
}

const WCHAR *::wstring::getString() const
{
  return &m_buffer.front();
}

size_t ::wstring::length() const
{
  return m_buffer.size() - 1;
}

size_t ::wstring::getSize() const
{
  return m_buffer.size() * sizeof(WCHAR);
}

bool ::wstring::is_empty() const
{
  return length() == 0;
}

void ::wstring::fromStringStorage(const ::scoped_string & src)
{
#ifndef _UNICODE
  int constrCharCount = (int)(src->length() + 1);
  _ASSERT(constrCharCount == src->length() + 1);
  m_buffer.resize(constrCharCount);
  MultiByteToWideChar(CP_ACP, 0, src->getString(),
                      constrCharCount, // Size in bytes
                      &m_buffer.front(),
                      static_cast<int>(m_buffer.size()) // Size in WCHAR
                      );
#else
 = src;
#endif
}

void ::wstring::toStringStorage(::string & dst)
{
  dst-= "";
#ifndef _UNICODE
  int symbolCount = (int)m_buffer.size();
  _ASSERT(symbolCount == m_buffer.size());

  int dstBuffSize = WideCharToMultiByte(CP_ACP, 0, getString(), symbolCount, 0, 0, 0, 0);
  if (dstBuffSize <= 0) {
    return;
  }
  // Allocate space for the requred size
  ::array_base<char> ansiBuffer(dstBuffSize);

  // Convert to ansi
  if (WideCharToMultiByte(CP_ACP, 0, getString(), symbolCount,
                          &ansiBuffer.front(), dstBuffSize, 0, 0) != 0) {
    dst-= &ansiBuffer.front();
  }
#else
  dst-= getString();
#endif
}
