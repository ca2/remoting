// Copyright (C) 2012 GlavSoft LLC.
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
#include "ConfigurationDialog.h"
#include "NamingDefs.h"
#include "remoting_impact.h"
#include "resource.h"
#include "acme/filesystem/file/item.h"
//#include "file_lib/::file::item.h"
#include "subsystem/node/Process.h"
#include "remoting/remoting_common/remoting.h"


namespace remoting_remoting
{
    ConfigurationDialog::ConfigurationDialog()
    :
      m_application(0)
    {
       initialize_dialog(IDD_CONFIGURATION);
    }

    void ConfigurationDialog::setListenerOfUpdate(::subsystem::OperatingSystemApplicationInterface *application)
    {
        m_application = application;
    }

    bool ConfigurationDialog::onCommand(unsigned int controlID, unsigned int notificationID)
    {
        if (controlID == IDC_EVERBLVL) {
            if (notificationID == EN_CHANGE) {
                onLogLevelChange();
            }
        }
        if (controlID == ::innate_subsystem::e_control_id_ok) {
            onOkPressed();
            if (m_application != 0) {
                m_application->postMessage(remoting_impact::WM_USER_CONFIGURATION_RELOAD);
            }
            closeDialog(1);
            return true;
        }
        if (controlID == ::innate_subsystem::e_control_id_cancel) {
            closeDialog(0);
            return true;
        }
        if (controlID == IDC_BCLEAR_LIST) {
            ::remoting::ViewerConfig::getInstance()->getConnectionHistory()->clear();
        }
        if (controlID == IDC_OPEN_LOG_FOLDER_BUTTON) {
            onOpenFolderButtonClick();
        }
        return false;
    }

    void ConfigurationDialog::onLogLevelChange()
    {
        ::string text;
        int logLevel;
        text = m_textboxVerbLvl.getText();
        main_subsystem()->string_parser()->parseInt(text, &logLevel);
        if (logLevel != 0) {
            m_textboxLogging.enableWindow(true);

            // If log-file is exist, then enable button "Locate...", else disable him.
            //::string logDir;
            auto logDir = ::remoting::ViewerConfig::getInstance()->getLogDir();
            auto logFileName = logDir / (::string(LogNames::VIEWER_LOG_FILE_STUB_NAME) + ".log");

            auto  logFile=file_item(logFileName);
            if (logFile->exists()) {
                m_controlOpenLogDir.enableWindow(true);
            } else {
                m_controlOpenLogDir.enableWindow(false);
            }
        } else {
            m_textboxLogging.enableWindow(false);
            m_controlOpenLogDir.enableWindow(false);
        }
    }
    void ConfigurationDialog::onOpenFolderButtonClick()
    {
        //::string logDir;

        auto logDir = ::remoting::ViewerConfig::getInstance()->getLogDir();

        auto pathLog = logDir / (::string(LogNames::VIEWER_LOG_FILE_STUB_NAME) + ".log");

        ::string command;

        command.format("explorer /select,{}", pathLog);

        ::subsystem::Process explorer;

       explorer.initialize_process(command);

        try {
            explorer.start();
        } catch (...) {
            // TODO: Place error notification here.
        }
    }

    bool ConfigurationDialog::onInitDialog()
    {
        dialog_item(m_checkboxShowToolBars, IDC_CSHOWTOOLBARS);
        dialog_item(m_checkboxWarnAtSwitching, IDC_CWARNATSW);
        dialog_item(m_textboxNumberConn, IDC_ENUMCON);
        dialog_item(m_spincontrolNumConn, IDC_SNUMCON);
        dialog_item(m_textboxReverseConn, IDC_EREVCON);
        dialog_item(m_spincontrolReverseConn, IDC_SREVCON);
        dialog_item(m_textboxVerbLvl, IDC_EVERBLVL);
        dialog_item(m_spincontrolVerbLvl, IDC_SVERBLVL);
        dialog_item(m_textboxLogging, IDC_ELOGGING);
        dialog_item(m_controlOpenLogDir, IDC_OPEN_LOG_FOLDER_BUTTON);

        m_spincontrolNumConn.setRange(0, 1024);
        m_spincontrolNumConn.setBuddy(&m_textboxNumberConn);

        m_spincontrolReverseConn.setRange32(1, 65535);
        m_spincontrolReverseConn.setBuddy(&m_textboxReverseConn);

        m_spincontrolVerbLvl.setRange(0, 9);
        m_spincontrolVerbLvl.setBuddy(&m_textboxVerbLvl);

        updateControlValues();

        return false;
    }

    void ConfigurationDialog::updateControlValues()
    {
        auto config = ::remoting::ViewerConfig::getInstance();

        ::string txt;

        txt.formatf("{}", config->getListenPort());
        m_textboxReverseConn.setText(txt);

        txt.formatf("{}", config->getLogLevel());
        m_textboxVerbLvl.setText(txt);

        txt.formatf("{}", config->getHistoryLimit());
        m_textboxNumberConn.setText(txt);

        m_checkboxShowToolBars.setChecked(config->isToolbarShown());
        m_checkboxWarnAtSwitching.setChecked(config->isPromptOnFullscreenEnabled());

        ::string logFileName;
        logFileName.formatf("{}\\{}.log", config->getPathToLogFile(), LogNames::VIEWER_LOG_FILE_STUB_NAME);
        m_textboxLogging.setText(logFileName);
    }

    bool ConfigurationDialog::isInputValid()
    {
        if (!testNum(&m_textboxReverseConn, main_subsystem()->string_table()->getString(IDS_CONFIGURATION_LISTEN_PORT))) {
            return false;
        }
        if (!testNum(&m_textboxVerbLvl, main_subsystem()->string_table()->getString(IDS_CONFIGURATION_LOG_LEVEL))) {
            return false;
        }
        if (!testNum(&m_textboxReverseConn, main_subsystem()->string_table()->getString(IDS_CONFIGURATION_HISTORY_LIMIT))) {
            return false;
        }
        return true;
    }

    bool ConfigurationDialog::testNum(innate_subsystem::TextBoxInterface *tb, const ::scoped_string & scopedstrTbName)
    {
        //::string text;
        auto text = tb->getText();

        if (main_subsystem()->string_parser()->tryParseInt(text))
        {
            return true;
        }

        ::string scopedstrMessage;
        scopedstrMessage.formatf(main_subsystem()->string_table()->getString(IDS_ERROR_VALUE_FIELD_ONLY_NUMERIC).c_str(), scopedstrTbName.c_str());

        main_subsystem()->message_box(operating_system_window(), scopedstrMessage,
                   main_subsystem()->string_table()->getString(IDS_CONFIGURATION_CAPTION), ::user::e_message_box_ok | ::user::e_message_box_icon_warning);

        tb->setFocus();

        return false;
    }

    void ConfigurationDialog::onOkPressed()
    {
        if (!isInputValid()) {
            return ;
        }

        auto config = ::remoting::ViewerConfig::getInstance();

        ::string text;
        int intVal;

        text = m_textboxReverseConn.getText();
        main_subsystem()->string_parser()->parseInt(text, &intVal);
        config->setListenPort(intVal);

        text = m_textboxVerbLvl.getText();
        main_subsystem()->string_parser()->parseInt(text, &intVal);
        config->setLogLevel((enum_trace_level)intVal);

        int oldLimit = config->getHistoryLimit();
        text=m_textboxNumberConn.getText();
        main_subsystem()->string_parser()->parseInt(text, &intVal);
        config->setHistoryLimit(intVal);

        if (config->getHistoryLimit() < oldLimit) {
            config->getConnectionHistory()->truncate();
        }

        config->showToolbar(m_checkboxShowToolBars.isChecked());
        config->promptOnFullscreen(m_checkboxWarnAtSwitching.isChecked());

        auto *sm = ::remoting::ViewerSettingsManager::getInstance();
        config->saveToStorage(sm);
    }


} // namespace remoting_remoting



