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
#include "../NewFolderDialog.h"
#include "../framework.h"
#include "../resource.h"
// #include "remoting/remoting_rtc/remoting.h"

namespace remoting_client
{
    NewFolderDialog::NewFolderDialog()
    {
        setResourceId(ftclient_createFolderDialog);
        m_strFileName= "";
    }

    NewFolderDialog::~NewFolderDialog()
    {
    }

   void NewFolderDialog::initialize_new_folder_dialog(::innate_subsystem::ControlInterface *parent)
    {
       setResourceId(ftclient_createFolderDialog);
       setParent(parent->impl<WindowInterface>());
       m_strFileName= "";
    }


    void NewFolderDialog::setFileName(const ::scoped_string & scopedstrFilename)
    {
        m_strFileName= scopedstrFilename;
    }

    ::string NewFolderDialog::getFileName()
    {
        return m_strFileName;
    }

    bool NewFolderDialog::onInitDialog()
    {
        initControls();
        m_textboxFileName.setText(m_strFileName);
        return true;
    }

    bool NewFolderDialog::onNotify(::u32 controlID, ::lparam data)
    {
        return true;
    }

    bool NewFolderDialog::onCommand(::u32 controlID, ::u32 notificationID)
    {
        switch (controlID) {
            case ::innate_subsystem::e_control_id_ok:
                onOkButtonClick();
                break;
            case ::innate_subsystem::e_control_id_cancel:
                onCancelButtonClick();
                break;
        }
        return true;
    }

    bool NewFolderDialog::onDestroy()
    {
        return true;
    }

    void NewFolderDialog::onOkButtonClick()
    {
        ::string fileName;

        fileName = m_textboxFileName.getText();

        if (fileName.is_empty() || fileName.contains_any_character_in("\\/"))
        {
            MainSubsystem().message_box(operating_system_window(),
                       L"::file::item name cannot be empty and cannot contain '/' or '\\' characters.",
                       L"Incorrect ::file::item Name",
                       ::user::e_message_box_ok | ::user::e_message_box_icon_warning);
            m_textboxFileName.setFocus();
            return ;
        }

        m_strFileName = fileName;

        closeDialog(::innate_subsystem::e_control_id_ok);
    }

    void NewFolderDialog::onCancelButtonClick()
    {
        closeDialog(::innate_subsystem::e_control_id_cancel);
    }

    void NewFolderDialog::initControls()
    {
        //HWND hwnd = m_hwnd;

        dialog_item(m_controlLabel, IDC_LABEL);
        dialog_item(m_textboxFileName, IDC_FILENAME_EDIT);

    }

}// namespace remoting_client
