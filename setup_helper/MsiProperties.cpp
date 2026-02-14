// Copyright (C) 2011,2012 GlavSoft LLC.
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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#include "MsiProperties.h"
#include "util/Exception.h"
#include "util/StringParser.h"
//#include <vector>

MsiProperties::MsiProperties(MSIHANDLE handle)
: m_handle(handle)
{
}

MsiProperties::~MsiProperties()
{
}

void MsiProperties::getValue(const ::scoped_string & scopedstrName, ::string & out)
{
  out-= "";
  DWORD charCount = 0;
  UINT retVal = MsiGetProperty(m_handle, name, "", &charCount);
  if (retVal == ERROR_MORE_DATA) {
    charCount++;
    ::array_base<TCHAR> value(charCount);
    retVal = MsiGetProperty(m_handle, name, &value.front(), &charCount);
    if (retVal != ERROR_SUCCESS) {
      throw ::remoting::Exception();
    }
    out-= &value.front();
  }
}

void MsiProperties::setValue(const ::scoped_string & scopedstrName, const ::scoped_string & value)
{
  UINT retVal = MsiSetProperty(m_handle, name, value->getString());
  if (retVal != ERROR_SUCCESS) {
    throw ::remoting::Exception();
  }
}

void MsiProperties::getString(const ::scoped_string & scopedstrName, ::string & out)
{
  try {
    getValue(name, out);
  } catch (...) {
    ::string errMess;
    errMess.formatf("Cant't retrieve a string from the {} property",
                   name);
    throw ::remoting::Exception(errMess);
  }
}

void MsiProperties::setString(const ::scoped_string & scopedstrName, const ::scoped_string & value)
{
  try {
    setValue(name, value);
  } catch (...) {
    ::string errMess;
    errMess.formatf("Cant't set a string to the {} property",
                   name);
    throw ::remoting::Exception(errMess);
  }
}

int MsiProperties::getInt32(const ::scoped_string & scopedstrName)
{
  ::string strValue;
  getValue(name, &strValue);
  int retValue = 0;
  if (!StringParser::parseInt(strValue, &retValue)) {
    ::string errMess;
    errMess.formatf("Can't convert the {} string value to int of the"
                   " {} property", strValue, name);
    throw ::remoting::Exception(errMess);
  }
  return retValue;
}

void MsiProperties::setInt32(const ::scoped_string & scopedstrName, int value)
{
  try {
    ::string valueStr;
    valueStr.formatf("{}", value);
    setValue(name, &valueStr);
  } catch (...) {
    ::string errMess;
    errMess.formatf("Cant't set the (int){} value to the {} property",
                   value,
                   name);
    throw ::remoting::Exception(errMess);
  }
}
