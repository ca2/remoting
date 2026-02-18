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
#include "FolderListener.h"

#include "acme/filesystem/filesystem/directory_system.h"
#include "acme/filesystem/filesystem/listing.h"
//#include <vector>

namespace  remoting
{
   namespace  ftp
   {
      FolderListener::FolderListener(::particle * pparticle, const ::file::path & pathFolder)
      //: m_filesInfo(NULL), m_filesCount(0)
      {
         m_pathFolder= pathFolder;
         initialize(pparticle);
      }

      FolderListener::~FolderListener()
      {
         // if (m_filesInfo != NULL) {
         //   delete[] m_filesInfo;
         // }
      }

      // const FileInfo *FolderListener::getFilesInfo() const
      // {
      //   return m_filesInfo;
      // }
      //
      // unsigned int FolderListener::getFilesCount() const
      // {
      //   return m_filesCount;
      // }

      bool FolderListener::list()
      {
         //::string fileNameList;
         //m_filesCount = 0;
         //bool listResult = true;

         //::file::listing listing;

         auto listing = directory_system()->folders_and_files(m_pathFolder);

         // if (!m_pathFolder.is_empty()) {
         //    ::file::item folder(m_pathFolder);
         //    stra = folder.list();
         // } else {
         //    stra = ::file::item::listRoots();
         // }

         if (listing.is_empty()) {
            return false;
         }

         // fileNameList = new ::string[m_filesCount];
         //
         // if (m_filesInfo != NULL) {
         //   delete[] m_filesInfo;
         //   m_filesInfo = NULL;
         // }

         // m_filesInfo = new FileInfo[m_filesCount];
         //
         // if (!m_pathFolder.is_empty()) {
         //   ::file::item folder(m_pathFolder);
         //   folder.list(fileNameList, NULL);
         // } else {
         //   ::file::item::listRoots(fileNameList, NULL);
         // }
         auto & pathFolder = m_pathFolder;

         for (unsigned int i = 0; i < listing.size(); i++)
         {

            auto & strName = listing[i];

            auto pfileitem = this->file_item( pathFolder/ strName);
            auto pfileinfo = øallocate FileInfo(pfileitem);

            m_fileinfoa.add(pfileinfo);

            //
            // All files in root folder is directories
            //

            if (m_pathFolder.is_empty()) {
               pfileinfo->setFlags(FileInfo::DIRECTORY);
               pfileinfo->setSize(0);
               pfileinfo->setLastModified(0);
            }
         }

         //delete[] fileNameList;

         return true;
      }
   } // namespace ftp
} // namespace remoting