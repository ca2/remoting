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

#ifndef _INI_FILE_SETTINGS_MANAGER_H_
#define _INI_FILE_SETTINGS_MANAGER_H_

#include "SettingsManager.h"

//#include "util/::string.h"

class IniFileSettingsManager : public SettingsManager
{
public:
  IniFileSettingsManager(const ::file::path & path, const ::scoped_string & scopedstrAppName);
  IniFileSettingsManager(const ::file::path & path);
  IniFileSettingsManager();

  virtual ~IniFileSettingsManager();

  // Sets application name (section name)
  void setApplicationName(const ::scoped_string & scopedstrAppName);

  // Sets path to ini file
  void setPathToFile(const ::file::path & path);

  //
  // Inherited from SettingsManager class
  //

  // Return false if path to ini file name not specified
  virtual bool isOk();

  virtual bool keyExist(const ::scoped_string & scopedstrName);

  virtual bool deleteKey(const ::scoped_string & scopedstrName);

  virtual bool getString(const ::scoped_string & scopedstrName, ::string & storage);
  // Remark: returns value if value is NULL.
  virtual bool setString(const ::scoped_string & scopedstrName, const ::scoped_string & scopedstrvalue);

  virtual bool getLong(const ::scoped_string & scopedstrName, long *value);
  virtual bool setLong(const ::scoped_string & scopedstrName, long value);

  virtual bool getBoolean(const ::scoped_string & scopedstrName, bool *value);
  virtual bool setBoolean(const ::scoped_string & scopedstrName, bool value);

  virtual bool getUINT(const ::scoped_string & scopedstrName, UINT *value);
  virtual bool setUINT(const ::scoped_string & scopedstrName, UINT value);

  virtual bool getInt(const ::scoped_string & scopedstrName, int *value);
  virtual bool setInt(const ::scoped_string & scopedstrName, int value);

  virtual bool getByte(const ::scoped_string & scopedstrName, char *value);
  virtual bool setByte(const ::scoped_string & scopedstrName, char value);

  virtual bool getBinaryData(const ::scoped_string & scopedstrName, void *value, size_t *size);
  virtual bool setBinaryData(const ::scoped_string & scopedstrName, const void *value, size_t size);

protected:
  ::string m_appName;
  ::file::path m_path;

private:

  // Helper method.
  // Sets string returned by GetPrivateProfileString to value out argument.
  //
  // Parameters:
  // [in]  name - key name
  // [out] value - variable where output string will be located
  // [in]  defaultValue -  if key does not exists defaultValue
  //       will be storaged to value argument as output value.
  ::string getPrivateProfileString(const ::scoped_string & scopedstrName, const ::scoped_string & scopedstrDefaultValue);

  //
  // Helper template method to avoid code duplicate
  // in such methods as getByte, getUINT.
  //

  template<typename T> bool getIntAndCastTo(const ::scoped_string & scopedstrName, T *value) {
    int intValue = 0;
    if (!getInt(scopedstrName, &intValue)) {
      return false;
    }
    *value = (T)intValue;
    return true;
  };

};

#endif
