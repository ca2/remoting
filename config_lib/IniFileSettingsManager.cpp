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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "IniFileSettingsManager.h"

#include <crtdbg.h>
#include <vector>

IniFileSettingsManager::IniFileSettingsManager(const ::file::path & path, const ::scoped_string & scopedstrAppName)
{
  m_path = path;
  m_appName = scopedstrAppName;
}

IniFileSettingsManager::IniFileSettingsManager(const ::file::path & path)
{
  m_path = path;
}

IniFileSettingsManager::IniFileSettingsManager()
{
}

IniFileSettingsManager::~IniFileSettingsManager()
{
}

void IniFileSettingsManager::setApplicationName(const ::scoped_string & scopedstrAppName)
{
  m_appName = scopedstrAppName;
}

void IniFileSettingsManager::setPathToFile(const ::scoped_string & scopedstrPathToFile)
{
  m_pathToFile = scopedstrPathToFile;
}

bool IniFileSettingsManager::isOk()
{
  return !m_pathToFile.is_empty();
}

bool IniFileSettingsManager::keyExist(const ::scoped_string & scopedstrName)
{
  //
  // To determinate key exists or not, place some non-standart default text
  // to default value of GetPrivateProfileString and if output returns is equals
  // to default, then key does not exists.
  //

  ::string value;

  // FIXME: Use random generated string instead of static text
  ::string strDefaultValue = _T("_Ini_File_Key_Does_Not_Exist_Test");

  getPrivateProfileString(scopedstrName, value, strDefaultValue);

  if (value == strDefaultValue) {
    return false;
  }
  return true;
}

bool IniFileSettingsManager::deleteKey(const ::scoped_string & scopedstrName)
{
  return WritePrivateProfileString(::wstring(m_appName), ::wstring(scopedstrName),
                                   NULL, m_path.windows_path()) == TRUE;
}

bool IniFileSettingsManager::getString(const ::scoped_string & scopedstrName, ::string & strStorage)
{
  if (!keyExist(scopedstrName)) {
    return false;
  }
  getPrivateProfileString(scopedstrName, strStorage, NULL);
  return true;
}

bool IniFileSettingsManager::setString(const ::scoped_string & scopedstrName, const ::scoped_string & scopedstrValue)
{
  if (scopedstrValue.is_empty()) {
    return false;
  }
  return WritePrivateProfileString(::wstring(m_appName), ::wstring(scopedstrName),
                                   ::wstring(scopedstrValue), m_path.windows_path()) == TRUE;
}

// FIXME: Stub
bool IniFileSettingsManager::getLong(const ::scoped_string & scopedstrName, long *value)
{
  _ASSERT(FALSE);
  return false;
}

// FIXME: Stub
bool IniFileSettingsManager::setLong(const ::scoped_string & scopedstrName, long value)
{
  _ASSERT(FALSE);
  return false;
}

bool IniFileSettingsManager::getBoolean(const ::scoped_string & scopedstrName, bool *value)
{
  int intVal;
  if (!getInt(scopedstrName, &intVal)) {
    return false;
  }
  *value = intVal == 1;
  return true;
}

bool IniFileSettingsManager::setBoolean(const ::scoped_string & scopedstrName, bool value)
{
  return setInt(scopedstrName, value ? 1 : 0);
}

bool IniFileSettingsManager::getUINT(const ::scoped_string & scopedstrName, UINT *value)
{
  return getIntAndCastTo<UINT>(scopedstrName, value);
}

bool IniFileSettingsManager::setUINT(const ::scoped_string & scopedstrName, UINT value)
{
  ::string stringVal;
  stringVal.format(_T("%u"), value);

  return setString(scopedstrName, stringVal);
}

bool IniFileSettingsManager::getInt(const ::scoped_string & scopedstrName, int *value)
{
  // We really cannot determinate result of GetPrivateProfileInt,
  // so use this trick, if returning value is defVal, than key does not
  // exists and method must return false.
  // FIXME: This trick will not work in some cases
  UINT defVal = 0xABCDEF;
  UINT ret = GetPrivateProfileInt(::wstring(m_appName), ::wstring(scopedstrName), defVal,
                                  m_path.windows_path());
  if (ret == defVal) {
    return false;
  }

  *value = (int)ret;

  return true;
}

bool IniFileSettingsManager::setInt(const ::scoped_string & scopedstrName, int value)
{
  ::string stringVal;
  stringVal.format(_T("%d"), value);

  return setString(name, stringVal.getString());
}

bool IniFileSettingsManager::getByte(const ::scoped_string & scopedstrName, char *value)
{
  return getIntAndCastTo<char>(name, value);
}

bool IniFileSettingsManager::setByte(const ::scoped_string & scopedstrName, char value)
{
  return setInt(name, value);
}

// FIXME: Stub
bool IniFileSettingsManager::getBinaryData(const ::scoped_string & scopedstrName, void *value, size_t *size)
{
  _ASSERT(FALSE);
  return false;
}

// FIXME: Stub
bool IniFileSettingsManager::setBinaryData(const ::scoped_string & scopedstrName, const void *value, size_t size)
{
  _ASSERT(FALSE);
  return false;
}

void IniFileSettingsManager::getPrivateProfileString(const ::scoped_string & scopedstrName,
                                                     ::string & value,
                                                     const ::scoped_string & scopedstrDefaultValue)
{
  ::std::vector<TCHAR> buffer;
  DWORD bufferSize = 1024;

  bool tooSmall = false;

  do {
    // Allocate buffer
    buffer.resize(bufferSize);

    // Try to get string value from storage
    DWORD ret = GetPrivateProfileString(m_appName.getString(), name,
                                        defaultValue, &buffer.front(), bufferSize,
                                        m_pathToFile.getString());

    // This mean that output buffer size is too small
    tooSmall = (ret == bufferSize - 1);
    bufferSize = bufferSize + bufferSize / 2;
  } while (tooSmall);

  value->setString(&buffer.front());
}
