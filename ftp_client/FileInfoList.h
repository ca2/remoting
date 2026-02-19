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
//#pragma once
//

#include "remoting/ftp_common/FileInfo.h"

namespace  remoting
{
   namespace  ftp
   {
      //
      // 2D two-side linked ::list_base class with FileInfo data inside
      // specified for using in recursive file transfer client operations
      // like recursive files removal, coping files trees etc.
      //
      //
      // Remark: class is owner of it m_next and m_child lists,
      // it must care about memory allocated for it.
      //
      // Class allocates memory for m_next ::list_base and m_child ::list_base
      // by own hands and frees memory when it's needed.
      //



      class FileInfoList :
      virtual public ::particle
      {
      public:

         //
         // Creates empty 2d two-side linked ::list_base with fileInfo data inside
         //

         FileInfoList(::particle * pparticle, FileInfo fileInfo);

         //
         // Creates 1d two-side linked ::list_base with data from filesInfo array
         //

         FileInfoList(::particle * pparticle, const ::pointer_array< FileInfo > & fileinfoa);
         //static FileInfoList *fromArray(const FileInfo *filesInfo, size_t count);

         //
         // Frees memory from next and child lists
         //

         ~FileInfoList();

         //
         // Creates new file info ::list_base from filesInfo array and set it to child
         // of this ::list_base (also it creates backward "child to parent" relationship).
         //

         void setChild(::particle * pparticle, const ::pointer_array < FileInfo > & fileinfoa);

         //
         // Returns child of this ::list_base or 0 if no child
         //

         FileInfoList *getChild();

         //
         // Returns parent of this ::list_base or 0 if no parent
         //

         FileInfoList *getParent();

         //
         // Returns top root (beggining of all ::list_base tree), cannot be 0, always valid ::list_base pointer
         //

         FileInfoList *getRoot();

         //
         // Returns top first (begging of this leaf) element in this ::list_base
         //

         FileInfoList *getFirst();

         //
         // Returns next ::list_base element or 0 if no next ::list_base
         //

         FileInfoList *getNext();

         //
         // Returns previous ::list_base element or 0 if no such ::list_base
         //

         FileInfoList *getPrev();

         //
         // Sets file info hold by this ::list_base
         //

         void setFileInfo(::particle * pparticle, FileInfo fileInfo);

         //
         // Returns file info hold by this ::list_base
         //

         FileInfo *getFileInfo();

         //
         // Sets absolute filename (calculated by parent files) associated
         // with hold file info in this ::list_base to storage variable.
         //
         // directorySeparator is char that used to split directories strings.
         //

         //void getAbsolutePath(::string & storage, TCHAR directorySeparator);
         static ::pointer < FileInfoList > create_from_array(::particle * pparticle, const ::pointer_array< FileInfo > & fileinfoa);
         ::file::path getAbsolutePath();

      //protected:

         void setNext(FileInfoList *next);
         void setPrev(FileInfoList *prev);


      //protected:

         ::pointer < FileInfoList > m_pchild;
         ::pointer < FileInfoList > m_pparent;

         ::pointer < FileInfoList > m_pnext;
         ::pointer < FileInfoList > m_pprev;

         FileInfo m_fileInfo;

      };


   }


}
//


