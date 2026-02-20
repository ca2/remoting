// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "acme/_operating_system.h"
#include "ResourceLoader.h"


#include <crtdbg.h>

ResourceLoader::ResourceLoader(HINSTANCE appInst)
: m_appInstance(appInst)
{
}

ResourceLoader::~ResourceLoader()
{
}

HICON ResourceLoader::loadStandartIcon(const ::scoped_string & scopedstriconName)
{
  return LoadIcon(NULL, ::wstring(scopedstriconName));
}

HICON ResourceLoader::loadIcon(const ::scoped_string & scopedstriconName)
{
  return LoadIcon(m_appInstance, ::wstring(scopedstriconName));
}

bool ResourceLoader::loadString(UINT id, ::string & str)
{
  //_ASSERT(string != 0);
  str= "(Undef)";

  //
  // Format of string table:
  // Strings are stored in groups of 16 strings.
  // Group format is:
  // | length  |     string     | length  | string  | ...
  // | 2 bytes | len * 2 bytes  | 2 bytes | len * 2 | ...
  // Strings stored in the UTF16-encoding.
  // 


  // Id of string-group, based from 0.
  int resId = (id / 16) + 1;
  HRSRC resHnd = FindResource(m_appInstance,
                              MAKEINTRESOURCE(resId),
                              RT_STRING);
  if (resHnd != 0) {
    HGLOBAL hGlobal = LoadResource(m_appInstance, resHnd);
    LPVOID lockRes = LockResource(hGlobal);

    WCHAR *lpStr = reinterpret_cast<WCHAR *>(lockRes);
    for (UINT i = 0; i < (id % 16); i++) {
      lpStr += 1 + static_cast<unsigned short>(lpStr[0]);
    }

    unsigned short strLen = static_cast<unsigned short>(lpStr[0]);

    ::array_base<WCHAR> strBuff;
    strBuff.resize(strLen + 1);
    memcpy(strBuff.data(), lpStr + 1, strLen * sizeof(WCHAR));
    strBuff[strLen] = L'\0';

    UnlockResource(lockRes);
    FreeResource(hGlobal);

    ::wstring wstr;
    wstr= strBuff.data();
    str = wstr;
  }
  return true;
}

HACCEL ResourceLoader::loadAccelerator(UINT id)
{
  return LoadAccelerators(m_appInstance,
                          MAKEINTRESOURCE(id)); 
}

HCURSOR ResourceLoader::loadStandardCursor(const ::scoped_string & scopedstrid)
{
  return LoadCursor(0, ::wstring(scopedstrid));
}

HCURSOR ResourceLoader::loadCursor(UINT id)
{
  return LoadCursor(m_appInstance, MAKEINTRESOURCE(id));
}
