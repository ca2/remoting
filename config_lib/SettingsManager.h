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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "remoting/util/CommonHeader.h"
//////#include "util/::string.h"

class SettingsManager
{
public:
  SettingsManager();
  virtual ~SettingsManager();
public:  
  virtual bool isOk() = 0;

  virtual bool keyExist(const ::scoped_string & scopedstrName) = 0;
  virtual bool deleteKey(const ::scoped_string & scopedstrName) = 0;

  virtual bool getString(const ::scoped_string & scopedstrName, ::string & storage) = 0;
  virtual bool setString(const ::scoped_string & scopedstrName, const ::scoped_string & scopedstrPayload) = 0;

  virtual bool getLong(const ::scoped_string & scopedstrName, long *value) = 0;
  virtual bool setLong(const ::scoped_string & scopedstrName, long value) = 0;

  virtual bool getBoolean(const ::scoped_string & scopedstrName, bool *value) = 0;
  virtual bool setBoolean(const ::scoped_string & scopedstrName, bool value) = 0;

  virtual bool getUINT(const ::scoped_string & scopedstrName, UINT *value) = 0;
  virtual bool setUINT(const ::scoped_string & scopedstrName, UINT value) = 0;

  virtual bool getInt(const ::scoped_string & scopedstrName, int *value) = 0;
  virtual bool setInt(const ::scoped_string & scopedstrName, int value) = 0;

  virtual bool getByte(const ::scoped_string & scopedstrName, char *value) = 0;
  virtual bool setByte(const ::scoped_string & scopedstrName, char value) = 0;

  virtual bool getBinaryData(const ::scoped_string & scopedstrName, void *value, size_t *size) = 0;
  virtual bool setBinaryData(const ::scoped_string & scopedstrName, const void *value, size_t size) = 0;
};


