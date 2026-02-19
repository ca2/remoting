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

#pragma once

//////#include "util/::string.h"
#include "util/inttypes.h"
//#include "file_lib/::file::item.h"

namespace remoting
{
   namespace ftp
   {
      //
      // Contains information about file in file transfer protocol
      // format.
      //

      class FileInfo :
      virtual public ::particle
      {
      public:

         //
         // Combination of flags that can be used in m_flags
         // class members
         //

         const static int DIRECTORY  = 0x1;
         const static int EXECUTABLE = 0x2;

      public:

         //
         // Creates FileInfo class with empty file information (no flags,
         // no size, name is set to "").
         //

         FileInfo();

         //
         // Creates FileInfo class with members with values specified
         // in constructor's arguments.
         //

         FileInfo(unsigned long long size, unsigned long long modTime,
                  unsigned short flags, const ::scoped_string & scopedstrFileName);

         //
         // Creates FileInfo class with name, size, flags that will be
         // retrieved from file argument.
         //

         FileInfo(const ::file::item *pfileitem);

         //
         // Returns true if DIRECTORY flag is set
         //

         bool isDirectory() const;

         //
         // Returns true if EXECUTABLE flag is set
         //

         bool isExecutable() const;

         //
         // Sets last modification time (in seconds from unix epoch)
         //

         void setLastModified(unsigned long long time);

         //
         // Sets file size (in bytes)
         //

         void setSize(unsigned long long size);

         //
         // Sets file flags (see static FileInfo constants)
         //

         void setFlags(unsigned short flags);

         //
         // Sets relative (from parent folder) file name
         //

         void setFileName(const ::scoped_string & scopedstrFileName);

         //
         // Returns file last modified time (in secords, starts from unix epoch)
         //

         unsigned long long lastModified() const;

         //
         // Returns file size in bytes
         //

         unsigned long long getSize() const;

         //
         // Returns file flags (see FileInfo static constants)
         //

         unsigned short getFlags() const;

         //
         // Returns file name
         //

         ::string getFileName() const;

         ///protected:
         unsigned long long m_sizeInBytes;
         class ::time  m_lastModified;
         unsigned short m_flags;
         ::string m_strFileName;
      };

      ///
   } // namespace ftp
} //namespace remoting