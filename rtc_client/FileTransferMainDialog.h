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


#include "innate_subsystem/gui/Dialog.h"
#include "innate_subsystem/gui/Control.h"
#include "innate_subsystem/gui/TextBox.h"
#include "innate_subsystem/gui/ComboBox.h"
#include "innate_subsystem/gui/ImagedButton.h"
#include "innate_subsystem/gui/ProgressBar.h"

#include "remoting/remoting_rtc/file_transfer_common/FileInfo.h"

#include "acme/exception/io.h"

#include "FileInfoListView.h"
#include "FileExistDialog.h"
#include "subsystem/thread/Thread.h"
#include "remoting/remoting_rtc/file_transfer_client/FileTransferInterface.h"

//#include aaa_<vector>
namespace remoting_client
{
    class FileTransferMainDialog : public ::innate_subsystem::Dialog,
                                   public ::remoting_rtc::file_transfer::FileTransferInterface
    {
    public:
        FileTransferMainDialog(::remoting_rtc::file_transfer::FileTransferCore *core);
        virtual ~FileTransferMainDialog();

        //
        // Kill main dialog.
        // Function return false, if closing dialog is canceled, and true else
        //
        bool tryClose();

        //
        // Inherited from FtInterface
        //
        ::i32 onFtTargetFileExists(::remoting_rtc::file_transfer::FileInfo *sourceFileInfo,
                                 ::remoting_rtc::file_transfer::FileInfo *targetFileInfo,
                                 const ::file::path & pathToTargetFile) override;
        void setProgress(::f64 progress) override;

        void onFtOpError(const ::scoped_string & scopedstrMessage) override;
        void onFtOpInfo(const ::scoped_string & scopedstrMessage) override;
        void onFtOpStarted() override;
        void onFtOpFinished(::i32 state, ::i32 result) override;

        //
        // filetransfer's operation is finished. Need update of control
        //
        void setNothingState() override;

        //
        // Called if local file ::list_base is updated
        //
        void onRefreshLocalFileList() override;

        // Called if remote file ::list_base is updated
        void onRefreshRemoteFileList() override;

        //
        // Shows error scopedstrMessage and throws exception
        //

        void raise(::exception &ex) override;

        //protected:

        //
        // Inherited from BaseDialog
        //

        bool onInitDialog() override;
        //virtual bool onNotify(::u32 controlID, ::lparam data);
        bool onCommand(::u32 controlID, ::u32 notificationID) override;
        bool onDestroy() override;

         bool _002OnAction(::i32 iControl) override;
         bool _002OnKeyDownNotification(::i32 iControl, ::user::enum_key ekey) override;
         bool _002OnColumnClick(::i32 iControl, ::i32 iColumn) override;
         bool _002OnSelectionChange(::i32 iControl) override;


        void onMessageReceived(::u32 uMsg, ::wparam wParam, ::lparam lParam) override;

        //
        // Button event handlers
        //

        void onCancelButtonClick();
        void onCancelOperationButtonClick();

        void onRenameRemoteButtonClick();
        void onMkDirRemoteButtonClick();
        void onRemoveRemoteButtonClick();
        void onRefreshRemoteButtonClick();

        void onRenameLocalButtonClick();
        void onMkDirLocalButtonClick();
        void onRemoveLocalButtonClick();
        void onRefreshLocalButtonClick();

        void onUploadButtonClick();
        void onDownloadButtonClick();

        void moveUpLocalFolder();
        void moveUpRemoteFolder();

        //
        // List view event handlers
        //

        void onRemoteListViewDoubleClick();
        void onLocalListViewDoubleClick();

        void onRemoteListViewKeyDown(::user::enum_key ekey);
        void onLocalListViewKeyDown(::user::enum_key ekey);

        //
        // Enables or disables rename and delete buttons
        // depending of file ::list_base views selected items count.
        //

        void checkRemoteListViewSelection();
        void checkLocalListViewSelection();

        //
        // Text notification methods
        //

        void insertMessageIntoComboBox(const ::scoped_string & scopedstrMessage);

        //private:

        //
        // Enables or disables all controls from m_controlsToBlock ::list_base
        //

        void enableControls(bool enabled);

        //
        // Links gui control class members with windows in dialog
        //

        void initControls();


        //
        // Refreshes local file ::list_base
        //

        void refreshLocalFileList();

        //
        // Refreshes remote file ::list_base
        //

        void refreshRemoteFileList();

        //
        // Displays file ::list_base of pathToFile folder of local machine
        // to local file ::list_base view
        //

        void tryListLocalFolder(const ::file::path & pathToFile);

        //
        // Sends file ::list_base request to server and shows result
        // in remote file ::list_base view
        //

        void tryListRemoteFolder(const ::file::path & pathToFile);

        //
        // Filenames helper methods
        //

        //
        // FIXME: Make classes for getPathTo*** methods
        //

        ::file::path getPathToCurrentLocalFolder();
        ::file::path getPathToParentLocalFolder();
        ::file::path getPathToSelectedLocalFile();

        ::file::path getPathToCurrentRemoteFolder();
        ::file::path getPathToParentRemoteFolder();
        ::file::path getPathToSelectedRemoteFile();

        //protected:
        //
        // True if window state is closing.
        //

        bool m_isClosing;



        ::string m_lastSentFileListPath;
        ::string m_lastReceivedFileListPath;

        //
        // Buttons
        //

        ::innate_subsystem::Control m_renameRemoteButton;
        ::innate_subsystem::Control m_mkDirRemoteButton;
        ::innate_subsystem::Control m_removeRemoteButton;
        ::innate_subsystem::Control m_refreshRemoteButton;

        ::innate_subsystem::Control m_renameLocalButton;
        ::innate_subsystem::Control m_mkDirLocalButton;
        ::innate_subsystem::Control m_removeLocalButton;
        ::innate_subsystem::Control m_refreshLocalButton;

        ::innate_subsystem::Control m_uploadButton;
        ::innate_subsystem::Control m_downloadButton;

        ::innate_subsystem::Control m_cancelButton;

        //
        // Progress bar
        //

        ::innate_subsystem::ProgressBar m_copyProgressBar;

        //
        // Combo box
        //

        ::innate_subsystem::ComboBox m_logComboBox;

        //
        // Text boxes
        //

        ::innate_subsystem::TextBox m_localCurFolderTextBox;
        ::innate_subsystem::TextBox m_remoteCurFolderTextBox;

        //
        // Tables
        //

        FileInfoListView m_localFileListView;
        FileInfoListView m_remoteFileListView;

        //
        // Helper modal dialogs
        //

        FileExistDialog m_fileExistDialog;

        //
        // ::file::item info of ".." fake folder
        //

        ::remoting_rtc::file_transfer:: FileInfo *m_fakeMoveUpFolder;

        //private:

        static const ::u32 WM_OPERATION_FINISHED = WM_USER + 2;
    };


} // namespace remoting_client


