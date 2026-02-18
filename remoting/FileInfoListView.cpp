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
#include "FileInfoListView.h"
//#include "util/::earth::time.h"
#include "remoting/resource.h"
#include "util/ResourceLoader.h"
#include <crtdbg.h>
#include <stdio.h>

#include "acme/prototype/datetime/datetime.h"


FileInfoListView::FileInfoListView()
: m_smallImageList(0)
{
  // By default, file ::list_base is sorted by file name.
  sort(0);
}

FileInfoListView::~FileInfoListView()
{
  if (m_smallImageList != 0) {
    ImageList_Destroy(m_smallImageList);
  }
}

//
// Saves hwnd and automaticly adds columns to ::list_base view
//

void FileInfoListView::setWindow(HWND hwnd)
{
  ListView::setWindow(hwnd);

  ListView::addColumn(0, "Name", 135);
  ListView::addColumn(1, "Size", 80, LVCFMT_RIGHT);
  ListView::addColumn(2, "Modified", 115);

  setFullRowSelectStyle(true);

  loadImages();

  ListView_SetImageList(m_hwnd, m_smallImageList, LVSIL_SMALL);

   subclass_window();

  //::remoting::Window::replaceWindowProc(FileInfoListView::s_newWndProc);
}

void FileInfoListView::addItem(int index, ::remoting::ftp::FileInfo *fileInfo)
{
  auto strFilename = fileInfo->getFileName();

  int imageIndex = IMAGE_FILE_INDEX;

  if (wcscmp(wstring(fileInfo->getFileName()), L".") == 0) {
    imageIndex = IMAGE_FOLDER_UP_INDEX;
  } else if (fileInfo->isDirectory()) {
    imageIndex = IMAGE_FOLDER_INDEX;
  }

  ListView::addItem(index, strFilename, (LPARAM)fileInfo, imageIndex);

  ::string sizeString("<Folder>");
  ::string modTimeString("");

  if (!fileInfo->isDirectory()) {
    //
    // Prepare size string
    //

    unsigned long long fileSize = fileInfo->getSize();

    if (fileSize <= 1024) {
      sizeString.formatf("{} B", fileSize);
    } else if ((fileSize > 1024) && (fileSize <= 1024 * 1024)) {
      sizeString.formatf("%4.2f KB", static_cast<double>(fileSize) / 1024.0);
    } else if (fileSize > 1024 * 1024) {
      sizeString.formatf("%4.2f MB", static_cast<double>(fileSize) / (1024.0 * 1024));
    }

    //
    // Prepare modification time string
    //

    ::earth::time dateTime(::posix_time(::posix_time_t{}, fileInfo->lastModified()));

    modTimeString = datetime()->date_time_text(dateTime);
  }

  ListView::setSubItemText(index, 1, sizeString);
  ListView::setSubItemText(index, 2, modTimeString);
}

//void FileInfoListView::addRange(::remoting::ftp::FileInfo **filesInfo, size_t count)
void FileInfoListView::addRange(const ::pointer_array < ::remoting::ftp::FileInfo > & fileinfoa)
{
  //int index = maximum(0, (getCount() - 1));
   auto i = maximum(0, getCount() - 1);
  //size_t i = 0;
  //::remoting::ftp::FileInfo *arr = *filesInfo;

  // Add folders first
  for (auto & pinfo : fileinfoa) {
    //::remoting::ftp::FileInfo *fi = &arr[i];
    if (pinfo->isDirectory()) {
      addItem(i++, pinfo);
    } // if directory
  } // for all files info

  // Add files
   for (auto & pinfo : fileinfoa) {
    //::remoting::ftp::FileInfo *fi = &arr[i];
    if (!pinfo->isDirectory()) {
      addItem(i++, pinfo);
    } // if not directory
  } // for all files info

  ListView::sort();
} // void

::pointer<::remoting::ftp::FileInfo > FileInfoListView::getSelectedFileInfo()
{
  int si = getSelectedIndex();
  if (si == -1) {
    return {};
  }
   auto pparticle = (::particle *) (void*) (::uptr)getSelectedItem().tag;
   ::cast < ::remoting::ftp::FileInfo> pfileinfo = pparticle;
  return pfileinfo;
}

void FileInfoListView::loadImages()
{
  if (m_smallImageList != NULL) {
    ImageList_Destroy(m_smallImageList);
  }

  m_smallImageList = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
                                      GetSystemMetrics(SM_CYSMICON),
                                      ILC_MASK, 1, 1);

  HICON icon;

  ResourceLoader *rLoader = ResourceLoader::getInstance();

  icon = rLoader->loadIcon(MAKEINTRESOURCE(IDI_FILEUP));
  _ASSERT(icon != NULL);
  ImageList_AddIcon(m_smallImageList, icon);
  DestroyIcon(icon);

  icon = rLoader->loadIcon(MAKEINTRESOURCE(IDI_FOLDER_ICON));
  _ASSERT(icon != NULL);
  ImageList_AddIcon(m_smallImageList, icon);
  DestroyIcon(icon);

  icon = rLoader->loadIcon(MAKEINTRESOURCE(IDI_FILE_ICON));
  _ASSERT(icon != NULL);
  ImageList_AddIcon(m_smallImageList, icon);
  DestroyIcon(icon);
}

void FileInfoListView::sort(int columnIndex)
{
  ListView::sort(columnIndex, compareItem);
}

int FileInfoListView::compareUInt64(unsigned long long first, unsigned long long second)
{
  if (first < second) {
    return -1;
  }
  if (first > second) {
    return 1;
  }
  return 0;
}

int FileInfoListView::compareItem(LPARAM lParam1,
                                   LPARAM lParam2,
                                   LPARAM lParamSort)
{
  // check ascending order
  bool sortAscending = lParamSort > 0;

  ::remoting::ftp::FileInfo *firstItem = reinterpret_cast<::remoting::ftp::FileInfo *>(lParam1);
  ::remoting::ftp::FileInfo *secondItem = reinterpret_cast<::remoting::ftp::FileInfo *>(lParam2);

  // Fake directory ".." should be into top ::list_base.
  if (wcscmp(::wstring(firstItem->getFileName()), L"..") == 0) {
    return -1;
  }

  if (wcscmp(::wstring(secondItem->getFileName()), L"..") == 0) {
    return 1;
  }

  // Directories should be upper, than files.
  if (firstItem->isDirectory() && !secondItem->isDirectory()) {
    return -1;
  }
  if (!firstItem->isDirectory() && secondItem->isDirectory()) {
    return 1;
  }
  
  // change lParam1 and lParam2 with each other if order is descending
  if (sortAscending) {
    firstItem = reinterpret_cast<::remoting::ftp::FileInfo *>(lParam1);
    secondItem = reinterpret_cast<::remoting::ftp::FileInfo *>(lParam2);
  } else {
    firstItem = reinterpret_cast<::remoting::ftp::FileInfo *>(lParam2);
    secondItem = reinterpret_cast<::remoting::ftp::FileInfo *>(lParam1);
  }

  if (lParamSort < 0) {
    // calculate column number when order is descending
    lParamSort = abs(lParamSort) - 1;
  } else {
    // calculate column number when order is ascending
    lParamSort -= 1;
  }
 
  switch (lParamSort) {
  // It's column "FileName".
  case 0:
    return _tcsicmp(::wstring(firstItem->getFileName()), wstring(secondItem->getFileName()));

  // It's column "FileSize".
  case 1:
    {
      // Size of directory is 0. Sort him by name.
      if (firstItem->isDirectory()) {
        return compareItem(lParam1, lParam2, 1);
      }
      int compareSize = compareUInt64(firstItem->getSize(), secondItem->getSize());
      // Sort by name, if sizes is equal.
      if (compareSize == 0) {
        return compareItem(lParam1, lParam2, 1);
      }
      return compareSize;
    }
  // It's column "Last modified".
  case 2:
    {
      int compareTime = compareUInt64(firstItem->lastModified(), secondItem->lastModified());
      // Sort by name, if time stamps is equal.
      if (compareTime == 0) {
        return compareItem(lParam1, lParam2, 1);
      }
      return compareTime;
    }
  // It's unknown column.
  default:
    _ASSERT(false);
    return 0;
  }
}

bool FileInfoListView::window_procedure(LRESULT & lresult, UINT uMsg, ::wparam wparam, ::lparam lparam)
{
  //FileInfoListView *_this = reinterpret_cast<FileInfoListView *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  switch (uMsg) {
  case WM_GETDLGCODE:
    lresult= CallWindowProc(m_defWindowProc, m_hwnd, uMsg, wparam.m_number, lparam.m_lparam);
    // We want WM_KEYDOWN scopedstrMessage when enter is pressed
    if (lparam.m_lparam &&
        ((MSG *)lparam.m_lparam)->message == WM_KEYDOWN &&
        ((MSG *)lparam.m_lparam)->wParam == VK_RETURN) {
      lresult = DLGC_WANTMESSAGE;
    }
    return true;
  } // switch

   return false;
  //return CallWindowProc(m_defWindowProc, hwnd, uMsg, wParam, lParam);
}
