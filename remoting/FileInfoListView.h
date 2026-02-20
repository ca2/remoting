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


#include "remoting/remoting_common/gui/ListView.h"
#include "remoting/remoting_common/ftp_common/FileInfo.h"

class FileInfoListView : public ListView
{
public:
  FileInfoListView();
  virtual ~FileInfoListView();

  virtual void setWindow(HWND hwnd);

  //
  // Adds new item to FileInfoListView
  //

  void addItem(int index, ::remoting::ftp::FileInfo *fileInfo);

  //
  // Adds files info array to the end of ::list_base view
  //

  //void addRange(::remoting::ftp::FileInfo **filesInfo, size_t count);
   void addRange(const ::pointer_array < ::remoting::ftp::FileInfo > & fileinfoa);

  //
  // Returns file info notated by first selected ::list_base view item
  //

  ::pointer < ::remoting::ftp::FileInfo >getSelectedFileInfo();

  void sort(int columnIndex);
//protected:

  //
  // Loads file ::list_base view icons from application resources
  //

  void loadImages();

  //
  // This function compare two item with file-contex (file name, date, size).
  //
  static int CALLBACK compareItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

  //
  // This function return:
  //   -1, if first < second
  //   0, if first == second
  //   1, if first > second
  //
  static int compareUInt64(unsigned long long first, unsigned long long second);

  HIMAGELIST m_smallImageList;

//private:
  bool window_procedure(LRESULT & lresult, UINT scopedstrMessage, ::wparam wparam, ::lparam lparam) override;

  static const int IMAGE_FOLDER_UP_INDEX = 0;
  static const int IMAGE_FOLDER_INDEX = 1;
  static const int IMAGE_FILE_INDEX = 2;
};


