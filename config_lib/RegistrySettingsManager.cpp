// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "RegistrySettingsManager.h"
#include "util/StringParser.h"
#include <vector>

RegistrySettingsManager::RegistrySettingsManager()
: m_key(0)
{
}

RegistrySettingsManager::RegistrySettingsManager(HKEY rootKey, const ::scoped_string & scopedstrEntry, SECURITY_ATTRIBUTES *sa)
: m_key(0)
{
  setRegistryKey(rootKey, scopedstrEntry, sa);
}

RegistrySettingsManager::~RegistrySettingsManager()
{
  if (m_key != NULL) {
    delete m_key;
  }
}

void RegistrySettingsManager::setRegistryKey(HKEY rootKey, const ::scoped_string & scopedstrEntry, SECURITY_ATTRIBUTES *sa)
{
  if (m_key != 0) {
    delete m_key;
  }

  m_key = new RegistryKey(rootKey, scopedstrEntry, true, sa);
}

bool RegistrySettingsManager::isOk()
{
  return (m_key != NULL) && (m_key->isOpened());
}

::string RegistrySettingsManager::key_path(const ::scoped_string & scopedstrKey)
{
   return scopedstrKey.rear_prefix('\\');
  // ::std::vector<TCHAR> folderString(scopedstrKey.length() + 1);
  // memcpy(&folderString.front(), key, folderString.size() * sizeof(TCHAR));
  // TCHAR *token = _tcsrchr(&folderString.front(), _T('\\'));
  // if (token != NULL) {
  //   *token = _T('\0');
  //   folder->setString(&folderString.front());
  // } else {
  //   folder->setString(_T(""));
  // }
}

::string RegistrySettingsManager::key_name(const ::scoped_string & scopedstrKey)
{
   return scopedstrKey.rear_word('\\');
  // ::std::vector<TCHAR> nameString(_tcslen(key) + 1);
  // memcpy(&nameString.front(), key, nameString.size() * sizeof(TCHAR));
  // TCHAR *token = _tcsrchr(&nameString.front(), _T('\\'));
  // if (token != NULL) {
  //   keyName->setString(++token);
  // } else {
  //   keyName->setString((TCHAR *)key);
  // }
}

bool RegistrySettingsManager::keyExist(const ::scoped_string & scopedstrName)
{
  if (!isOk()) return false;
  RegistryKey subKey(m_key, name, false);
  return subKey.isOpened();
}

bool RegistrySettingsManager::deleteKey(const ::scoped_string & scopedstrName)
{
  ::string keyName;
  ::string valueName;

  extractKeyName(name, &keyName);
  extractValueName(name, &valueName);

  RegistryKey subKey(m_key, keyName.getString(), false);

  bool deleteAsSubKey = m_key->deleteSubKeyTree(valueName.getString());
  bool deleteAsValue = subKey.deleteValue(valueName.getString());

  return deleteAsSubKey || deleteAsValue;
}

bool RegistrySettingsManager::getString(const ::scoped_string & scopedstrName, ::string & value)
{
  ::string keyName;
  ::string valueName;

  extractKeyName(name, &keyName);
  extractValueName(name, &valueName);

  RegistryKey subKey(m_key, keyName.getString(), false);

  return subKey.getValueAsString(valueName.getString(), value);
}

bool RegistrySettingsManager::setString(const ::scoped_string & scopedstrName, const ::scoped_string & scopedstrvalue)
{
  ::string keyName;
  ::string valueName;

  extractKeyName(name, &keyName);
  extractValueName(name, &valueName);

  RegistryKey subKey(m_key, keyName.getString(), false);

  return subKey.setValueAsString(name, value);
}

bool RegistrySettingsManager::getLong(const ::scoped_string & scopedstrName, long *value)
{
  ::string keyName;
  ::string valueName;

  extractKeyName(name, &keyName);
  extractValueName(name, &valueName);

  RegistryKey subKey(m_key, keyName.getString(), false);

  return subKey.getValueAsInt64(valueName.getString(), value);
}

bool RegistrySettingsManager::setLong(const ::scoped_string & scopedstrName, long value)
{
  ::string keyName;
  ::string valueName;

  extractKeyName(name, &keyName);
  extractValueName(name, &valueName);

  RegistryKey subKey(m_key, keyName.getString(), false);

  return subKey.setValueAsInt64(name, value);
}

bool RegistrySettingsManager::getBoolean(const ::scoped_string & scopedstrName, bool *value)
{
  int intVal = 0;
  if (!getInt(name, &intVal)) {
    return false;
  }
  *value = intVal == 1;
  return true;
}

bool RegistrySettingsManager::setBoolean(const ::scoped_string & scopedstrName, bool value)
{
  return setInt(name, value ? 1 : 0);
}

bool RegistrySettingsManager::getUINT(const ::scoped_string & scopedstrName, UINT *value)
{
  return getInt(name, (int *)value);
}

bool RegistrySettingsManager::setUINT(const ::scoped_string & scopedstrName, UINT value)
{
  return setInt(name, (int)value);
}

bool RegistrySettingsManager::getInt(const ::scoped_string & scopedstrName, int *value)
{
  ::string keyName;
  ::string valueName;

  extractKeyName(name, &keyName);
  extractValueName(name, &valueName);

  RegistryKey subKey(m_key, keyName.getString(), false);

  return subKey.getValueAsInt32(valueName.getString(), value);
}

bool RegistrySettingsManager::setInt(const ::scoped_string & scopedstrName, int value)
{
  ::string keyName;
  ::string valueName;

  extractKeyName(name, &keyName);
  extractValueName(name, &valueName);

  RegistryKey subKey(m_key, keyName.getString(), false);

  return subKey.setValueAsInt32(name, value);
}

bool RegistrySettingsManager::getByte(const ::scoped_string & scopedstrName, char *value)
{
  int intVal = 0;
  if (!getInt(name, &intVal)) {
    return false;
  }
  *value = (char)intVal;
  return true;
}

bool RegistrySettingsManager::setByte(const ::scoped_string & scopedstrName, char value)
{
  return setInt(name, (int)value);
}

bool RegistrySettingsManager::getBinaryData(const ::scoped_string & scopedstrName, void *value, size_t *size)
{
  ::string keyName;
  ::string valueName;

  extractKeyName(name, &keyName);
  extractValueName(name, &valueName);

  RegistryKey subKey(m_key, keyName.getString(), false);

  return subKey.getValueAsBinary(name, value, size);
}

bool RegistrySettingsManager::setBinaryData(const ::scoped_string & scopedstrName, const void *value, size_t size)
{
  ::string keyName;
  ::string valueName;

  extractKeyName(name, &keyName);
  extractValueName(name, &valueName);

  RegistryKey subKey(m_key, keyName.getString(), false);

  return subKey.setValueAsBinary(name, value, size);
}
