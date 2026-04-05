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


#include "apex/innate_subsystem/BaseDialog.h"
#include "apex/innate_subsystem/Control.h"
#include "remoting/remoting_common/ftp_common/FileInfo.h"


namespace remoting_remoting
{


    class FileExistDialog : public ::innate_subsystem::BaseDialog
    {
    public:

        //
        // Dialog result contants
        //

        static const int OVERWRITE_RESULT = 0x0;
        static const int SKIP_RESULT = 0x1;
        static const int APPEND_RESULT = 0x2;
        static const int CANCEL_RESULT = 0x3;

        FileExistDialog();
        ~FileExistDialog();

        // Override BaseDialog method
        virtual int showModal();

        void setFilesInfo(::remoting::ftp::FileInfo *existingFileInfo, ::remoting::ftp::FileInfo *newFileInfo,
                          const ::scoped_string & scopedstrPathToFileCaption);

        void resetDialogResultValue();

    //protected:

        //
        // Inherited from BaseDialog
        //

        virtual bool onInitDialog();
        virtual bool onNotify(unsigned int controlID, ::lparam data);
        virtual bool onCommand(unsigned int controlID, unsigned int notificationID);
        virtual bool onDestroy();

        //
        // Button event handlers
        //

        virtual void onOverwriteButtonClick();
        virtual void onOverwriteAllButtonClick();
        virtual void onSkipButtonClick();
        virtual void onSkipAllButtonClick();
        virtual void onAppendButtonClick();
        virtual void onCancelButtonClick();

        //private:

        virtual void updateGui(::remoting::ftp::FileInfo *fileInfo, ::innate_subsystem::Control *sizeLabel, ::innate_subsystem::Control *modTimeLabel);
        virtual void initControls();

        //protected:
        ::remoting::ftp::FileInfo *m_newFileInfo;
        ::remoting::ftp:: FileInfo *m_existingFileInfo;
        ::string m_pathToFileCaption;

        //
        // Helper members
        //

        bool m_controlsInitialized;
        bool m_overwriteAll;
        bool m_skipAll;

        //
        // User controls
        //

        ::pointer < ::innate_subsystem::Control > m_pcontrolFileNameLabel;
        ::pointer < ::innate_subsystem::Control > m_pcontrolNewSizeLabel;
        ::pointer < ::innate_subsystem::Control > m_pcontrolNewModTimeLabel;
        ::pointer < ::innate_subsystem::Control > m_pcontrolExistingSizeLabel;
        ::pointer < ::innate_subsystem::Control > m_pcontrolExistingModTimeLabel;

        ::pointer < ::innate_subsystem::Control > m_pcontrolAppendButton;

        bool m_canAppend;
    };


} // namespace remoting_remoting



