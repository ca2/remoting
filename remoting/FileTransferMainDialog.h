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


#include "subsystem_apex/Dialog.h"
#include "subsystem_apex/Control.h"
#include "subsystem_apex/TextBox.h"
#include "subsystem_apex/ComboBox.h"
#include "subsystem_apex/ImagedButton.h"
#include "subsystem_apex/ProgressBar.h"

#include "remoting/remoting_common/ftp_common/FileInfo.h"

#include "acme/exception/io.h"

#include "FileInfoListView.h"
#include "FileExistDialog.h"
#include "subsystem_acme/thread/Thread.h"
#include "remoting/remoting_common/ftp_client/FileTransferInterface.h"

//#include aaa_<vector>
namespace remoting_remoting
{
    class FileTransferMainDialog : public ::subsystem_apex::Dialog,
                                   public ::remoting::ftp::FileTransferInterface
    {
    public:
        FileTransferMainDialog(::remoting::ftp::FileTransferCore *core);
        virtual ~FileTransferMainDialog();

        //
        // Kill main dialog.
        // Function return false, if closing dialog is canceled, and true else
        //
        bool tryClose();

        //
        // Inherited from FtInterface
        //
        int onFtTargetFileExists(::remoting::ftp::FileInfo *sourceFileInfo,
                                 ::remoting::ftp::FileInfo *targetFileInfo,
                                 const ::file::path & pathToTargetFile);
        void setProgress(double progress);

        void onFtOpError(const ::scoped_string & scopedstrMessage);
        void onFtOpInfo(const ::scoped_string & scopedstrMessage);
        void onFtOpStarted();
        void onFtOpFinished(int state, int result);

        //
        // filetransfer's operation is finished. Need update of control
        //
        void setNothingState();

        //
        // Called if local file ::list_base is updated
        //
        void onRefreshLocalFileList();

        // Called if remote file ::list_base is updated
        void onRefreshRemoteFileList();

        //
        // Shows error scopedstrMessage and throws exception
        //

        void raise(::exception &ex) override;

        //protected:

        //
        // Inherited from BaseDialog
        //

        virtual bool onInitDialog();
        //virtual bool onNotify(unsigned int controlID, ::lparam data);
        virtual bool onCommand(unsigned int controlID, unsigned int notificationID);
        virtual bool onDestroy();

         bool _002OnAction(int iControl) override;
         bool _002OnKeyDownNotification(int iControl, ::user::enum_key ekey) override;
         bool _002OnColumnClick(int iControl, int iColumn) override;
         bool _002OnSelectionChange(int iControl) override;


        virtual void onMessageReceived(unsigned int uMsg, ::wparam wParam, ::lparam lParam);

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

        ::subsystem_apex::Control m_renameRemoteButton;
        ::subsystem_apex::Control m_mkDirRemoteButton;
        ::subsystem_apex::Control m_removeRemoteButton;
        ::subsystem_apex::Control m_refreshRemoteButton;

        ::subsystem_apex::Control m_renameLocalButton;
        ::subsystem_apex::Control m_mkDirLocalButton;
        ::subsystem_apex::Control m_removeLocalButton;
        ::subsystem_apex::Control m_refreshLocalButton;

        ::subsystem_apex::Control m_uploadButton;
        ::subsystem_apex::Control m_downloadButton;

        ::subsystem_apex::Control m_cancelButton;

        //
        // Progress bar
        //

        ::subsystem_apex::ProgressBar m_copyProgressBar;

        //
        // Combo box
        //

        ::subsystem_apex::ComboBox m_logComboBox;

        //
        // Text boxes
        //

        ::subsystem_apex::TextBox m_localCurFolderTextBox;
        ::subsystem_apex::TextBox m_remoteCurFolderTextBox;

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

        ::remoting::ftp:: FileInfo *m_fakeMoveUpFolder;

        //private:

        static const unsigned int WM_OPERATION_FINISHED = WM_USER + 2;
    };


} // namespace remoting_remoting


