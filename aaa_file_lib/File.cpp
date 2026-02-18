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
#include "::file::item.h"
#include "acme/operating_system/time.h"
//#include "util/::earth::time.h"
#include <crtdbg.h>

TCHAR ::file::item::s_separatorChar = _T('\\');

::file::item::::file::item(const ::file::path & path)
{
  m_path = path;
}

::file::item::::file::item(const ::scoped_string & scopedstrFolder, const ::scoped_string & scopedstrN)
{
  m_path = scopedstrFolder/scopedstrN;
  //
  // if (!m_path.ends(::file::item::s_separatorChar)) {
  //
  //   TCHAR separator[2];
  //   separator[0] = ::file::item::s_separatorChar;
  //   separator[1] = '\0';
  //
  //   m_path.appendString(&separator[0]);
  // }
  //
  // m_path.appendString(child);
}

::file::item::~::file::item()
{
}

bool ::file::item::canRead() const
{
  return tryCreateFile(GENERIC_READ, OPEN_EXISTING);
}

bool ::file::item::canWrite() const
{
  return tryCreateFile(GENERIC_WRITE, OPEN_EXISTING);
}

bool ::file::item::createNewFile() const
{
  return tryCreateFile(GENERIC_READ | GENERIC_WRITE, CREATE_NEW);
}

bool ::file::item::remove() const
{
  if (!exists()) {
    return false;
  }
  if ((isDirectory()) && (RemoveDirectory(::wstring(m_path)) != 0)) {
    return true;
  }
  if (isFile() && DeleteFile(::wstring(m_path)) != 0) {
    return true;
  }
  return false;
}

bool ::file::item::exists() const
{
  WIN32_FIND_DATA fileInfo;
  return getFileInfo(&fileInfo);
}

::string ::file::item::getName() const
{
   return m_path.name();
  // if (m_path.length() == 0) {
  //   //name-= "";
  //   return {};
  // }
  //
  // auto path = m_path;
  // size_t i = m_path.length();
  //
  // for (; i > 0; i--) {
  //   if (buffer[i - 1] == ::file::item::s_separatorChar) {
  //     break;
  //   } // if found separator
  // } // for every char in string
  //
  // m_path.getSubstring(name, i, m_path.length() - 1);
}

::string ::file::item::getFileExtension() const
{
   return m_path.final_extension();
  // ::string fileName;
  //
  // getName(&fileName);
  //
  // size_t pointPos = fileName.findLast(_T('.'));
  //
  // if (pointPos == (size_t)-1) {
  //   ext-= "";
  // } else {
  //   fileName.getSubstring(ext, pointPos + 1, fileName.length() - 1);
  // }
}

bool ::file::item::isFile() const
{
  if (exists() && !isDirectory()) {
    return true;
  }
  return false;
}

bool ::file::item::isDirectory() const
{
  WIN32_FIND_DATA fileInfo;
  if (getFileInfo(&fileInfo)) {
    if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      return true;
    } // if normal file
  } // if can get file info
  return false;
}

::file::path ::file::item::getPath() const
{
  //*pathname = m_path;
   return m_path;
}

unsigned long long ::file::item::lastModified() const
{
  WIN32_FIND_DATA fileInfo;

  if (!getFileInfo(&fileInfo)) {
    return 0;
  }

  ::earth::time dt;

   dt.m_iSecond = as_time(fileInfo.ftLastWriteTime);

  return dt.m_iSecond;
}

unsigned long long ::file::item::length() const
{
  WIN32_FIND_DATA fileInfo;

  if (!getFileInfo(&fileInfo)) {
    return 0;
  }

  long long maxDWORDPlusOne = 1 + (long long)MAXDWORD;

  return fileInfo.nFileSizeHigh * maxDWORDPlusOne + fileInfo.nFileSizeLow;
}

::string_array ::file::item::list() const
{
   ::string_array stra;
   auto pathPattern = m_path/"*";
  //folderPath.appendString("\\*");

  //unsigned int index = 0;

  HANDLE hfile;
  WIN32_FIND_DATA findFileData;

  //
  // Change error mode to avoid windows error scopedstrMessage in scopedstrMessage box
  // when we attemt to find first file on unmounted device
  //

  UINT savedErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);

  hfile = FindFirstFile(pathPattern.windows_path(), &findFileData);

  // Restore error mode
  SetErrorMode(savedErrorMode);

  if (hfile == INVALID_HANDLE_VALUE) {
    throw ::exception(error_failed);
  }

  do {

    //
    // Skip "fake" file names
    //

    if (wcscmp(findFileData.cFileName, L".") == 0 ||
        wcscmp(findFileData.cFileName, L"..") == 0) {
      continue;
    }

    //if (fileList != NULL) {
      //fileList[index]= findFileData.cFileName;
    //}
     stra.add(findFileData.cFileName);

    //index++;

  } while (FindNextFile(hfile, &findFileData));

  FindClose(hfile);

  //if (fileList == NULL) {
    //*filesCount = index;
  //}

  //return true;
return stra;
}


::string_array ::file::item::listRoots()
{

   ::string_array stra;
  TCHAR drivesList[256];

  if (GetLogicalDriveStrings(255, drivesList) == 0) {
    return stra;
  }

  //unsigned int count = 0;
  size_t i = 0;

  while (drivesList[i] != '\0') {

    TCHAR *drive = _tcsdup(&drivesList[i]);
    TCHAR *backslash = _tcsrchr(drive, '\\');

    if (backslash != NULL) {
      *backslash = '\0';
    }

    //if (rootList != NULL) {
      //rootList[count]= drive;
    //}
     stra.add(drive);

    free(drive);

    i += wcscspn(&drivesList[i], L"\0") + 1;
    //count++;
  } // while

  // if (rootList == NULL) {
  //   *rootsCount = count;
  // }

  return stra;
}

bool ::file::item::mkdir() const
{
  if (CreateDirectory(m_path.windows_path(), NULL) == 0) {
    return false;
  }
  return true;
}

bool ::file::item::renameTo(const ::file::path & path)
{
  ::file::item destFile(path);
  return renameTo(&destFile);
}

bool ::file::item::renameTo(::file::item *dest)
{
  //::string destPathName;
  auto path = dest->getPath();
  if (MoveFile(m_path.windows_path(), path.windows_path()) == 0) {
    return false;
  }
  return true;
}

bool ::file::item::renameTo(const ::file::path & pathTarget, const ::file::path & pathSource)
{
  // Try delete the destination file. Any error will be ignored.
  DeleteFile(pathTarget.windows_path());
  if (MoveFile(pathSource.windows_path(), pathTarget.windows_path()) == 0)
   {
    return false;
  }
  return true;
}

bool ::file::item::setLastModified(long long time)
{
  _ASSERT(time >= 0);

  HANDLE hfile = CreateFile(m_path.windows_path(),
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
  if (hfile == INVALID_HANDLE_VALUE) {
    return false;
  }

  //
  // Convert unix time to windows file time structure
  //

  FILETIME ft;

  ::earth::time dt((unsigned long long)time);

  dt.toFileTime(&ft);

  if (SetFileTime(hfile, NULL, NULL, &ft) == FALSE) {
    CloseHandle(hfile);
    return false;
  }

  CloseHandle(hfile);

  return true;
}

bool ::file::item::truncate()
{
  if (exists()) {
    if (!remove()) {
      return false;
    }
  }
  return createNewFile();
}

bool ::file::item::getFileInfo(WIN32_FIND_DATA *fileInfo) const
{
  HANDLE fileHandle;

  //
  // Change error mode to avoid windows error scopedstrMessage in scopedstrMessage box
  // when we attemt to find first file on unmounted device
  //

  UINT savedErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);

  fileHandle = FindFirstFile(m_path.windows_path(), fileInfo);

  // Restore error mode
  SetErrorMode(savedErrorMode);

  if (fileHandle == INVALID_HANDLE_VALUE) {
    return false;
  } // if file exists and we get file info

  FindClose(fileHandle);
  return true;
}

bool ::file::item::tryCreateFile(DWORD desiredAccess, DWORD creationDisposition) const
{
  HANDLE hfile = CreateFile(m_path.windows_path(),
                            desiredAccess,
                            FILE_SHARE_READ,
                            NULL,
                            creationDisposition,
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
                            NULL);

  if (hfile == INVALID_HANDLE_VALUE) {
    return false;
  }

  CloseHandle(hfile);

  return true;
}
