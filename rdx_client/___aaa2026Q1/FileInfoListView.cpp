// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
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
#include "../FileInfoListView.h"
#include "../framework.h"
// #include "subsystem/platform/class ::time.h"
#include "remoting/remoting_rtc/resource.h"
#include "subsystem/platform/ResourceLoader.h"
////#include aaa_<crtdbg.h>
//// #include aaa_<stdio.h>
#include "../resource.h"
#include "acme/prototype/datetime/datetime.h"
#include "innate_subsystem/gui/SystemMetrics.h"
#include "innate_subsystem/platform/ResourceLoader.h"
#include "innate_subsystem/platform/subsystem.h"


namespace remoting_client
{
    FileInfoListView::FileInfoListView()
    //: m_pimagelistSmall(0)
    {
        // By default, file ::list_base is sorted by file name.
        set_sort(0);
    }

    FileInfoListView::~FileInfoListView()
    {
        // if (m_pimagelistSmall != 0) {
        //     ImageList_Destroy(m_pimagelistSmall);
        // }
    }

    //
    // Saves hwnd and automaticly adds columns to ::list_base view
    //


       bool FileInfoListView::we_want_WM_KEYDOWN_when_enter_is_pressed() const
    {

       return true;

    }

    void FileInfoListView::subclassWindow(const ::operating_system::window & operatingsystemwindow)
    {
        ListView::subclassWindow(operatingsystemwindow);

        ListView::addColumn(0, "Name", 135);
        //ListView::addColumn(1, "Size", 80, LVCFMT_RIGHT);
       ListView::addColumn(1, "Size", 80, 0);
        ListView::addColumn(2, "Modified", 115);

        setFullRowSelectStyle(true);

        loadImages();

        //ListView_SetImageList(m_hwnd, m_pimagelistSmall, LVSIL_SMALL);

        //subclassWindow();

        //::innate_subsystem::Control::replaceWindowProc(FileInfoListView::s_newWndProc);
    }

    void FileInfoListView::addItem(::i32 index, ::remoting_rtc::file_transfer::FileInfo *fileInfo)
    {
        auto strFilename = fileInfo->getFileName();

        ::i32 imageIndex = IMAGE_FILE_INDEX;

        if (wcscmp(wstring(fileInfo->getFileName()), L".") == 0) {
            imageIndex = IMAGE_FOLDER_UP_INDEX;
        } else if (fileInfo->isDirectory()) {
            imageIndex = IMAGE_FOLDER_INDEX;
        }

        ListView::addItem(index, strFilename, (::lparam)fileInfo, imageIndex);

        ::string sizeString("<Folder>");
        ::string modTimeString("");

        if (!fileInfo->isDirectory()) {
            //
            // Prepare size string
            //

            ::u64 fileSize = fileInfo->getSize();

            if (fileSize <= 1024) {
                sizeString.format("{} B", fileSize);
            } else if ((fileSize > 1024) && (fileSize <= 1024 * 1024)) {
                sizeString.format("%4.2f KB", static_cast<::f64>(fileSize) / 1024.0);
            } else if (fileSize > 1024 * 1024) {
                sizeString.format("%4.2f MB", static_cast<::f64>(fileSize) / (1024.0 * 1024));
            }

            //
            // Prepare modification time string
            //

            class ::time dateTime(::posix_time(::posix_time_t{}, fileInfo->lastModified()));

            modTimeString = datetime()->date_time_text(dateTime);
        }

        ListView::setSubItemText(index, 1, sizeString);
        ListView::setSubItemText(index, 2, modTimeString);
    }

    //void FileInfoListView::addRange(::remoting_rtc::file_transfer::FileInfo **filesInfo, size_t count)
    void FileInfoListView::addRange(const ::pointer_array < ::remoting_rtc::file_transfer::FileInfo > & fileinfoa)
    {
        //::i32 index = maximum(0, (getCount() - 1));
        auto i = maximum(0, getCount() - 1);
        //size_t i = 0;
        //::remoting_rtc::file_transfer::FileInfo *arr = *filesInfo;

        // Add folders first
        for (auto & pinfo : fileinfoa) {
            //::remoting_rtc::file_transfer::FileInfo *fi = &arr[i];
            if (pinfo->isDirectory()) {
                addItem(i++, pinfo);
            } // if directory
        } // for all files info

        // Add files
        for (auto & pinfo : fileinfoa) {
            //::remoting_rtc::file_transfer::FileInfo *fi = &arr[i];
            if (!pinfo->isDirectory()) {
                addItem(i++, pinfo);
            } // if not directory
        } // for all files info

        ListView::sort();
    } // void

    ::pointer<::remoting_rtc::file_transfer::FileInfo > FileInfoListView::getSelectedFileInfo()
    {
        ::i32 si = getSelectedIndex();
        if (si == -1) {
            return {};
        }
        auto pparticle = (::particle *) (void*) (::uptr)getSelectedItem().tag;
        ::cast < ::remoting_rtc::file_transfer::FileInfo> pfileinfo = pparticle;
        return pfileinfo;
    }

    void FileInfoListView::loadImages()
    {
        if (m_pimagelistSmall != NULL) {
            // ImageList_Destroy(m_pimagelistSmall);

           m_pimagelistSmall->destroyImageList();
        }


       // = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
       //                                      GetSystemMetrics(SM_CYSMICON),
       //                                      ILC_MASK, 1, 1);

       auto psystemmetrics = InnateSubsystem().metrics();

       auto sizeSmallIconInPixels = psystemmetrics->get_small_icon_size_in_pixels();

      m_pimagelistSmall->createImageList(sizeSmallIconInPixels, ::innate_subsystem::image_list::e_create_mask);

        //HICON icon;

        auto presourceloader = InnateSubsystem().ResourceLoader();

        auto picon = presourceloader->loadIconByIntResource(IDI_FILEUP);
        ASSERT(picon != NULL);
        m_pimagelistSmall->addIcon(picon);
        //DestroyIcon(icon);

        picon = presourceloader->loadIconByIntResource(IDI_FOLDER_ICON);
        ASSERT(picon != NULL);
        m_pimagelistSmall->addIcon( picon);
        //DestroyIcon(icon);

        picon = presourceloader->loadIconByIntResource(IDI_FILE_ICON);
        ASSERT(picon != NULL);
        m_pimagelistSmall->addIcon(picon);
//        DestroyIcon(icon);
    }

    void FileInfoListView::set_sort(::i32 columnIndex)
    {
        ListView::set_sort(columnIndex, [this](::lparam lparam1, ::lparam lparam2)->::i32
        {

           auto lParamSort = lparam_sort();

           ::i32 iCompare = compareItem(lparam1, lparam2, lParamSort);

           return iCompare;

        });
    }

    ::i32 FileInfoListView::compareUInt64(::u64 first, ::u64 second)
    {
        if (first < second) {
            return -1;
        }
        if (first > second) {
            return 1;
        }
        return 0;
    }

   // ::i32  FileInfoListView::s_compareItem(::lparam lParam1, ::lparam lParam2, ::lparam lParamSort)
   //  {
   //
   //     auto pcompare = (compare_t *) lParamSort.m_lparam;
   //
   //     ::i32 iCompare= pcompare->m_pfileinfolistview->compareItem(lParam1, lParam2, pcompare);
   //
   //     return iCompare;
   //
   //  }
   //
    ::i32 FileInfoListView::compareItem(::lparam lParam1,
                                       ::lparam lParam2,
                                       ::lparam lParamSort)
    {

//       auto pcompare = (compare_t*) pCompare;

  //     auto lParamSort = pcompare->m_lparamSort;
        // check ascending order
        bool sortAscending = lParamSort < 0;

        ::remoting_rtc::file_transfer::FileInfo *firstItem = (::remoting_rtc::file_transfer::FileInfo *)(lParam1.m_lparam);
        ::remoting_rtc::file_transfer::FileInfo *secondItem = (::remoting_rtc::file_transfer::FileInfo *)(lParam2.m_lparam);

        // Fake directory ".." should be into top ::list_base.
        if (firstItem->getFileName() == "..")
         {
            return -1;
        }

        if (secondItem->getFileName() == L"..")
           {
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
           firstItem = (::remoting_rtc::file_transfer::FileInfo *)(lParam1.m_lparam);
           secondItem = (::remoting_rtc::file_transfer::FileInfo *)(lParam2.m_lparam);
        } else {
           firstItem = (::remoting_rtc::file_transfer::FileInfo *)(lParam1.m_lparam);
           secondItem = (::remoting_rtc::file_transfer::FileInfo *)(lParam2.m_lparam);

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
                return firstItem->getFileName().case_insensitive_compare(secondItem->getFileName());

                // It's column "FileSize".
            case 1:
            {
                // Size of directory is 0. Sort him by name.
                if (firstItem->isDirectory()) {
                    return compareItem(lParam1, lParam2, 1);
                }
                ::i32 compareSize = compareUInt64(firstItem->getSize(), secondItem->getSize());
                // Sort by name, if sizes is equal.
                if (compareSize == 0) {
                    return compareItem(lParam1, lParam2, 1);
                }
                return compareSize;
            }
                // It's column "Last modified".
            case 2:
            {
                ::i32 compareTime = compareUInt64(firstItem->lastModified(), secondItem->lastModified());
                // Sort by name, if time stamps is equal.
                if (compareTime == 0) {
                    return compareItem(lParam1, lParam2, 1);
                }
                return compareTime;
            }
                // It's unknown column.
            default:
                ASSERT(false);
                return 0;
        }
    }

    bool FileInfoListView::window_procedure(::lresult & lresult, ::u32 uMsg, ::wparam wparam, ::lparam lparam)
    {
        //FileInfoListView *_this = reinterpret_cast<FileInfoListView *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));


        return false;
        //return CallWindowProc(m_defWindowProc, hwnd, uMsg, wParam, lParam);
    }
}//namespace remoting_client
