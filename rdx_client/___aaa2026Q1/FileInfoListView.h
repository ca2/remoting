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

#pragma once


#include "innate_subsystem/gui/ImageList.h"
#include "innate_subsystem/gui/ListView.h"
#include "remoting/remoting_rtc/file_transfer_common/FileInfo.h"

namespace remoting_client
{


    class FileInfoListView : public ::innate_subsystem::ListView
    {
    public:

        FileInfoListView();
        ~FileInfoListView() override;


       virtual bool we_want_WM_KEYDOWN_when_enter_is_pressed() const override;

        void subclassWindow(const ::operating_system::window & operatingsystemwindow) override;

        //
        // Adds new item to FileInfoListView
        //

        virtual void addItem(::i32 index, ::remoting_rtc::file_transfer::FileInfo *fileInfo);

        //
        // Adds files info array to the end of ::list_base view
        //

        //void addRange(::remoting_rtc::file_transfer::FileInfo **filesInfo, size_t count);
        virtual void addRange(const ::pointer_array < ::remoting_rtc::file_transfer::FileInfo > & fileinfoa);

        //
        // Returns file info notated by first selected ::list_base view item
        //

        virtual ::pointer < ::remoting_rtc::file_transfer::FileInfo >getSelectedFileInfo();

        virtual void set_sort(::i32 columnIndex);
        //protected:

        //
        // Loads file ::list_base view icons from application resources
        //

        virtual void loadImages();

        //
        // This function compare two item with file-contex (file name, date, size).
        //
        //static ::i32  s_compareItem(::lparam lParam1, ::lparam lParam2, ::lparam lParamSort);

        virtual ::i32  compareItem(::lparam lParam1, ::lparam lParam2, ::lparam lParamSort);

        //
        // This function return:
        //   -1, if first < second
        //   0, if first == second
        //   1, if first > second
        //
        virtual ::i32 compareUInt64(::u64 first, ::u64 second);

        ::pointer < ::innate_subsystem::ImageListInterface > m_pimagelistSmall;

        //private:
        virtual bool window_procedure(::lresult & lresult, ::u32 scopedstrMessage, ::wparam wparam, ::lparam lparam);

        static const ::i32 IMAGE_FOLDER_UP_INDEX = 0;
        static const ::i32 IMAGE_FOLDER_INDEX = 1;
        static const ::i32 IMAGE_FILE_INDEX = 2;
    };
} // namespace remoting_client


