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
#include "FileInfoList.h"

namespace remoting
{
   namespace ftp
   {
      FileInfoList::FileInfoList(::particle * pparticle, FileInfo fileInfo)
//      : m_pparent(0), m_pchild(0), m_pnext(0), m_pprev(0)
      {
         setFileInfo(pparticle, fileInfo);
      }

      FileInfoList::FileInfoList(::particle * pparticle, const ::pointer_array< FileInfo > & fileinfoa)
      //: m_pparent(0), m_pchild(0), m_pnext(0), m_pprev(0)
      {
         setFileInfo(pparticle, *fileinfoa.first());

         auto plist = create_from_array(pparticle, fileinfoa);

         auto psecond = plist->getNext();

         //plist->m_pnext = NULL;

         if (psecond)
         {
            psecond->m_pprev = this;
         }

         m_pnext = psecond;

         ///delete plist;
      }

      FileInfoList::~FileInfoList()
      {
         //
         // This class is owner of child and next ::list_base elements
         // so in destructor it must free allocated memory.
         //

         // if (m_pchild != NULL) {
         //    delete m_pchild;
         // }
         // if (m_pnext != NULL) {
         //    delete m_pnext;
         // }
      }

      void FileInfoList::setNext(FileInfoList *next)
      {
         // if (m_pnext != NULL) {
         //    delete m_pnext;
         // }
         m_pnext = next;
      }

      void FileInfoList::setPrev(FileInfoList *prev)
      {
         // if (m_pprev != NULL) {
         //    delete m_pprev;
         // }
         m_pprev = prev;
      }

      void FileInfoList::setChild(::particle * pparticle, const ::pointer_array< FileInfo > & fileinfoa)
      {
         // // Cleanup
         // if (m_pchild != NULL) {
         //    delete m_pchild;
         // }
         // Parent to child relationship
         m_pchild = øallocate FileInfoList(pparticle, fileinfoa);
         // Child to parent relationship
         //if (m_pchild != NULL) {
            m_pchild->m_pparent = this;
         //}
      }

      FileInfoList *FileInfoList::getChild()
      {
         return m_pchild;
      }

      FileInfoList *FileInfoList::getParent()
      {
         return m_pparent;
      }

      FileInfoList *FileInfoList::getRoot()
      {
         // In our ::list_base only first element of leaf can has parent
         FileInfoList *first = getFirst();
         // first have no parent, so first is root
         if (first->getParent() == NULL) {
            return first;
         }
         // continue searching root ::list_base
         return first->getParent()->getRoot();
      }

      FileInfoList *FileInfoList::getFirst()
      {
         // If have no prev, than this is first element of leaf
         if (getPrev() == NULL) {
            return this;
         }
         // continue searching
         return getPrev()->getFirst();
      }

      FileInfoList *FileInfoList::getNext()
      {
         return m_pnext;
      }

      FileInfoList *FileInfoList::getPrev()
      {
         return m_pprev;
      }

      void FileInfoList::setFileInfo(::particle * pparticle, FileInfo fileInfo)
      {
         initialize(pparticle);
         m_fileInfo = fileInfo;
      }

      FileInfo *FileInfoList::getFileInfo()
      {
         return &m_fileInfo;
      }


      ::file::path FileInfoList::getAbsolutePath()
      {
         FileInfoList *first = getFirst();

         ::file::path pathParentAbsolute;

         //
         // if parent of first element of this leaf exists
         // than absolute path prefix is parent absolute path with
         // added directory separator symbol in the end.
         //

         bool firstHasParent = (first != NULL) && (first->getParent() != NULL);

         if (firstHasParent)
         {
            FileInfoList *firstParent = first->getParent();
            pathParentAbsolute = firstParent->getAbsolutePath();
         } // if first has parent

         auto strName = m_fileInfo.getFileName();

         auto path = pathParentAbsolute / strName;

         return path;

      }


      ::pointer < FileInfoList > FileInfoList::create_from_array(::particle * pparticle, const ::pointer_array < FileInfo > & fileinfoa)
      {
         if (fileinfoa.is_empty()) {
            return nullptr;
         }

         ::pointer < FileInfoList >pcurrent;
         ::pointer < FileInfoList > pprev;

         for (unsigned int i = 0; i < fileinfoa.size(); i++) {
            pcurrent = øallocate FileInfoList(pparticle, *fileinfoa[i]);
            pcurrent->setPrev(pprev);
            if (pprev) {
               pprev->setNext(pcurrent);
            }
            pprev = pcurrent;
         }

         return pcurrent->getFirst();
      }
   }
}