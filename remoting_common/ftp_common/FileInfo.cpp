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
#include "FileInfo.h"
#include "acme/filesystem/file/item.h"


namespace  remoting
{
   namespace ftp
   {
      FileInfo::FileInfo()
      : m_sizeInBytes(0), m_lastModified(0), m_flags(0)
      {
      }

      FileInfo::FileInfo(unsigned long long size, unsigned long long modTime,
                         unsigned short flags, const ::scoped_string & scopedstrFileName)
      : m_sizeInBytes(size), m_lastModified(modTime), m_flags(flags)
      {
         m_strFileName= scopedstrFileName;
      }

      //
      // FIXME: EXECUTABLE flag isn't sets now
      //

      FileInfo::FileInfo(const ::file::item *pfileitem)
      {
         if (pfileitem->is_folder())
         {
            m_flags |= FileInfo::DIRECTORY;
            m_sizeInBytes = 0;
            m_lastModified = 0;
         }
         else
         {
            m_flags = 0;
            m_sizeInBytes = pfileitem->size();
            m_lastModified = pfileitem->last_modified_time();
         }
         m_strFileName = pfileitem->final_path().name();
      }

      bool FileInfo::isDirectory() const
      {
         return (m_flags & FileInfo::DIRECTORY) ? true : false;
      }

      bool FileInfo::isExecutable() const
      {
         return (m_flags & FileInfo::EXECUTABLE) ? true : false;
      }

      void FileInfo::setLastModified(unsigned long long time)
      {
         m_lastModified = time;
      }

      void FileInfo::setSize(unsigned long long size)
      {
         m_sizeInBytes = size;
      }

      void FileInfo::setFlags(unsigned short flags)
      {
         m_flags = flags;
      }

      void FileInfo::setFileName(const ::scoped_string & scopedstrFileName)
      {
         m_strFileName= scopedstrFileName;
      }

      unsigned long long FileInfo::lastModified() const
      {
         return m_lastModified.m_iSecond;
      }

      unsigned long long FileInfo::getSize() const
      {
         return m_sizeInBytes;
      }

      unsigned short FileInfo::getFlags() const
      {
         return m_flags;
      }

      ::string FileInfo::getFileName() const
      {
         return m_strFileName;
      }
   } // namespace  ftp
} // namespace remoting

