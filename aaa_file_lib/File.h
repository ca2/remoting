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

//#pragma once
//
#pragma once

#include "remoting/util/CommonHeader.h"
#include "util/inttypes.h"

class ::file::item
{
public:

  const static int MODE_READ  = 1;
  const static int MODE_WRITE = 2;

  ::file::item(const ::file::path & path);
  ::file::item(const ::scoped_string & scopedstrFolder, const ::scoped_string & scopedstrSon);
  ~::file::item();

  //
  // Tests whether the application can read the file denoted by this pathname
  //

  bool canRead() const;

  //
  // Tests whether the application can read the file denoted by this pathname
  //

  bool canWrite() const;

  //
  // Atomically creates a new, empty file named by this pathname
  // if and only if a file with this name does not yet exist.
  //

  bool createNewFile() const;

  //
  // Deletes the file or directory denoted by this pathname
  //

  bool remove() const;

  //
  // Tests whether the file or directory denoted by this pathname exists
  //

  bool exists() const;

  /**
  Returns the name of the file or directory denoted by this pathname
  (not full path to file, only filename).
  @param name out parameter for file name.
  */
  ::string getName() const;

  /**
  Returns extension of file denoted by this pathname (without "." character).
  @param ext out parameter for file extension.
  @remark if filename has no extension, then ext will contain empty string("").
  */
  ::string getFileExtension() const;

  /**
  Tests whether the file denoted by this pathname is a normal file.
  @return true if file is a normal file (not directory), false otherwise.
  */
  bool isFile() const;

  /**
  Tests whether the file denoted by this abstract pathname is a directory.
  @return true if this file is directory, false otherwise.
  */
  bool isDirectory() const;

  /**
  Returns full path to file or directory.
  @param pathname out parameter for path to file or directory.
  */
  ::file::path getPath() const;

  /**
   * Returns the last modification time of file denoted by this pathname.
   * @return count of milliseconds since unix epoch or 0 on fail.
   * @fixme raise exception on fail.
   */
  unsigned long long lastModified() const;

  /**
   * Returns the length of the file (in bytes) denoted by this pathname.
   * @fixme raise exception on fail.
   */
  unsigned long long length() const;

  //
  // Fills fileList array of strings naming the files and directories
  // in the directory denoted by this pathname
  //
  // If fileList is NULL than sets files count value to filesCount
  //
  // FIXME: Current implementation forces to perform two calls, first to
  //        determine the ::list_base size, then to fill in the ::list_base itself.
  //        However, the ::list_base size can be changed between these calls!
  //        This MUST be fixed someday.
  //

  ::string_array list() const;

  //
  // List the available filesystem roots to rootList array
  //
  // If rootList is NULL that sets root ::list_base size to rootsCount
  //
  // FIXME: Current implementation forces to perform two calls, first to
  //        determine the ::list_base size, then to fill in the ::list_base itself.
  //        However, the ::list_base size can be changed between these calls!
  //        This MUST be fixed someday.
  //

  static ::string_array listRoots();

  //
  // Creates the directory named by this pathname
  //

  bool mkdir() const;

  //
  // Renames the file denoted by this pathname.
  // Really this methods call move method.
  //

  bool renameTo(const ::file::path & pathTarget);
  bool renameTo(::file::item *dest);
  static bool renameTo(const ::file::path & pathTarget, const ::file::path & pathSource);

  /**
   * Sets the last-modified time of the file or directory named by this pathname.
   * @param time count of milliseconds since unix epoch.
   * @return true if ok, false if fail.
   */

  bool setLastModified(long long time);

  //
  // Truncates file, removes file if it exists and creates new file with same pathname
  //

  bool truncate();

private:

  //
  // Helper methods to minimize code of ::file::item class
  //

  bool getFileInfo(WIN32_FIND_DATA *fileInfo) const;
  bool tryCreateFile(DWORD desiredAccess, DWORD creationDisposition) const;

//public:
  static TCHAR s_separatorChar;

//protected:
  //::string m_pathName;
   ::file::path m_path;
};

//
