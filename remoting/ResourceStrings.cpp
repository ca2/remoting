// Copyright (C) 2012 GlavSoft LLC.
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
#include "ResourceStrings.h"
#include "remoting/remoting_common/util/ResourceLoader.h"

ResourceStrings::ResourceStrings() 
: m_min(0), 
  m_max(0)
{
}

ResourceStrings::ResourceStrings(int min, int max) 
{
  m_min = min;
  m_max = max;
}

bool ResourceStrings::isValid(int res) 
{
  if (res >= m_min && res <= m_max) {
    return true;
  }
  return false;
}

::string ResourceStrings::getStrRes(int strRes) 
{
  ResourceLoader *rLoader = ResourceLoader::getInstance();

  ::string str;
  rLoader->loadString(strRes, str);
  return str;
}

::string ResourceStrings::getStrPureRes(int strRes) 
{
  ::string strTemp = getStrRes(strRes);
  auto p = strTemp.find_first_character('\t');
  if (::found(p)) {
    strTemp = {p + 1, strTemp.end()};
  }
  strTemp.erase_any_character_in("&");
  return strTemp;
}
