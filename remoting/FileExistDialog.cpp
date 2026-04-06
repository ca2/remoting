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
#include "framework.h"
#include "FileExistDialog.h"
//#include "remoting/remoting_common/util/::earth::time.h"
#include "resource.h"
#include <crtdbg.h>
#include <stdio.h>
#include "acme/prototype/datetime/datetime.h"


namespace remoting_remoting
{
    FileExistDialog::FileExistDialog()
    : m_controlsInitialized(false),
      m_newFileInfo(NULL),
      m_existingFileInfo(NULL),
      m_canAppend(true)
    {
        setResourceId(ftclient_fileExistDialog);
        resetDialogResultValue();
    }

    FileExistDialog::~FileExistDialog()
    {
    }

    int FileExistDialog::showModal()
    {
        if (m_skipAll) {
            return SKIP_RESULT;
        } else if (m_overwriteAll) {
            return OVERWRITE_RESULT;
        }
        return Dialog::showModal();
    }

    void FileExistDialog::setFilesInfo(::remoting::ftp::FileInfo *existingFileInfo, ::remoting::ftp::FileInfo *newFileInfo,
                                       const ::scoped_string & scopedstrPathToFileCaption)
    {
        m_newFileInfo = newFileInfo;
        m_existingFileInfo = existingFileInfo;

        m_pathToFileCaption= scopedstrPathToFileCaption;

        _ASSERT(m_newFileInfo != NULL);
        _ASSERT(m_existingFileInfo != NULL);

        m_canAppend = (m_existingFileInfo->getSize() < m_newFileInfo->getSize());

        if (m_controlsInitialized) {
            updateGui(newFileInfo, m_pcontrolNewSizeLabel, m_pcontrolNewModTimeLabel);
            m_pcontrolFileNameLabel->setText(scopedstrPathToFileCaption);
            m_pcontrolAppendButton->setEnabled(m_canAppend);
        }
    }

    void FileExistDialog::resetDialogResultValue()
    {
        m_overwriteAll = false;
        m_skipAll = false;
    }

    bool FileExistDialog::onInitDialog()
    {
        initControls();

        _ASSERT(m_newFileInfo != NULL);
        _ASSERT(m_existingFileInfo != NULL);

        updateGui(m_newFileInfo, m_pcontrolNewSizeLabel, m_pcontrolNewModTimeLabel);
        updateGui(m_existingFileInfo, m_pcontrolExistingSizeLabel, m_pcontrolExistingModTimeLabel);

        m_pcontrolAppendButton->setEnabled(m_canAppend);

        m_pcontrolFileNameLabel->setText(m_pathToFileCaption);

        return true;
    }

    bool FileExistDialog::onNotify(unsigned int controlID, ::lparam data)
    {
        return true;
    }

    bool FileExistDialog::onCommand(unsigned int controlID, unsigned int notificationID)
    {
        switch (controlID) {
            case IDC_OVERWRITE_BUTTON:
                onOverwriteButtonClick();
                break;
            case IDC_OVERWRITE_ALL_BUTTON:
                onOverwriteAllButtonClick();
                break;
            case IDC_SKIP_BUTTON:
                onSkipButtonClick();
                break;
            case IDC_SKIP_ALL_BUTTON:
                onSkipAllButtonClick();
                break;
            case IDC_APPEND_BUTTON:
                onAppendButtonClick();
                break;
            case IDC_CANCEL_BUTTON:
                onCancelButtonClick();
                break;
        }
        return true;
    }

    bool FileExistDialog::onDestroy()
    {
        return true;
    }

    void FileExistDialog::onOverwriteButtonClick()
    {
        closeDialog(OVERWRITE_RESULT);
    }

    void FileExistDialog::onOverwriteAllButtonClick()
    {
        closeDialog(OVERWRITE_RESULT);
        m_overwriteAll = true;
    }

    void FileExistDialog::onSkipButtonClick()
    {
        closeDialog(SKIP_RESULT);
    }

    void FileExistDialog::onSkipAllButtonClick()
    {
        closeDialog(SKIP_RESULT);
        m_skipAll = true;
    }

    void FileExistDialog::onAppendButtonClick()
    {
        closeDialog(APPEND_RESULT);
    }

    //
    // FIXME: Really it cannot break operation execution for now
    //

    void FileExistDialog::onCancelButtonClick()
    {
        closeDialog(CANCEL_RESULT);
    }

    void FileExistDialog::updateGui(::remoting::ftp::FileInfo *fileInfo, ::innate_subsystem::Control *sizeLabel, ::innate_subsystem::Control * modTimeLabel)
    {
        char buffer[255];
        #ifdef WINDOWS
        sprintf_s(&buffer[0], 255, "%lld bytes", fileInfo->getSize());
        #else
        sprintfs(&buffer[0], "%lld bytes", fileInfo->getSize());
        #endif
        sizeLabel->setText(&buffer[0]);

        ::earth::time dateTime(::posix_time(::posix_time_t{}, fileInfo->lastModified()));

        ::string formatTimeString;
        formatTimeString = datetime()->date_time_text(dateTime);

        modTimeLabel->setText(formatTimeString);
    }

    void FileExistDialog::initControls()
    {
        //HWND hwnd = m_hwnd;

        //m_pcontrolFileNameLabel->setWindow(GetDlgItem(hwnd, IDC_FILENAME_LABEL));
        //m_pcontrolNewSizeLabel->setWindow(GetDlgItem(hwnd, IDC_SIZE1_LABEL));
        //m_pcontrolNewModTimeLabel->setWindow(GetDlgItem(hwnd, IDC_DATE1_LABEL));
        //m_pcontrolExistingSizeLabel->setWindow(GetDlgItem(hwnd, IDC_SIZE2_LABEL));
        //m_pcontrolExistingModTimeLabel->setWindow(GetDlgItem(hwnd, IDC_DATE2_LABEL));

        //m_pcontrolAppendButton->setWindow(GetDlgItem(hwnd, IDC_APPEND_BUTTON));

        m_controlsInitialized = true;
    }


} // namespace remoting_remoting


