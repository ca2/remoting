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

#ifndef _FILE_INFO_LIST_H_
#define _FILE_INFO_LIST_H_

#include "ft_common/FileInfo.h"

//
// 2D two-side linked ::std::list class with FileInfo data inside
// specified for using in recursive file transfer client operations
// like recursive files removal, coping files trees etc.
//
//
// Remark: class is owner of it m_next and m_child lists,
// it must care about memory allocated for it.
//
// Class allocates memory for m_next ::std::list and m_child ::std::list
// by own hands and frees memory when it's needed.
//

class FileInfoList
{
public:

  //
  // Creates empty 2d two-side linked ::std::list with fileInfo data inside
  //

  FileInfoList(FileInfo fileInfo);

  //
  // Creates 1d two-side linked ::std::list with data from filesInfo array
  //

  FileInfoList(const FileInfo *filesInfo, size_t count);

  //
  // Frees memory from next and child lists
  //

  ~FileInfoList();

  //
  // Creates new file info ::std::list from filesInfo array and set it to child
  // of this ::std::list (also it creates backward "child to parent" relationship).
  //

  void setChild(const FileInfo *filesInfo, size_t count);

  //
  // Returns child of this ::std::list or 0 if no child
  //

  FileInfoList *getChild();

  //
  // Returns parent of this ::std::list or 0 if no parent
  //

  FileInfoList *getParent();

  //
  // Returns top root (beggining of all ::std::list tree), cannot be 0, always valid ::std::list pointer
  //

  FileInfoList *getRoot();

  //
  // Returns top first (begging of this leaf) element in this ::std::list
  //

  FileInfoList *getFirst();

  //
  // Returns next ::std::list element or 0 if no next ::std::list
  //

  FileInfoList *getNext();

  //
  // Returns previous ::std::list element or 0 if no such ::std::list
  //

  FileInfoList *getPrev();

  //
  // Sets file info hold by this ::std::list
  //

  void setFileInfo(FileInfo fileInfo);

  //
  // Returns file info hold by this ::std::list
  //

  FileInfo *getFileInfo();

  //
  // Sets absolute filename (calculated by parent files) associated
  // with hold file info in this ::std::list to storage variable.
  //
  // directorySeparator is char that used to split directories strings.
  //

  void getAbsolutePath(::string & storage, TCHAR directorySeparator);

protected:

  void setNext(FileInfoList *next);
  void setPrev(FileInfoList *prev);

  static FileInfoList *fromArray(const FileInfo *filesInfo, size_t count);

protected:

  FileInfoList *m_child;
  FileInfoList *m_parent;

  FileInfoList *m_next;
  FileInfoList *m_prev;

  FileInfo m_fileInfo;
};

#endif
