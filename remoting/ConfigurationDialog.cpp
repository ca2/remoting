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
#include "acme/subsystem/node/Process.h"
#include "remoting/remoting_common/remoting.h"


namespace remoting_remoting
{
    ConfigurationDialog::ConfigurationDialog()
    : Dialog(IDD_CONFIGURATION),
      m_application(0)
    {
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
        if (controlID == ::innate_subsystem::IDOK) {
            onOkPressed();
            if (m_application != 0) {
                m_application->postMessage(remoting_impact::WM_USER_CONFIGURATION_RELOAD);
            }
            closeDialog(1);
            return true;
        }
        if (controlID == ::innate_subsystem::IDCANCEL) {
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
        text = m_verbLvl.getText();
        main_subsystem()->string_parser()->parseInt(text, &logLevel);
        if (logLevel != 0) {
            m_logging.enableWindow(true);

            // If log-file is exist, then enable button "Locate...", else disable him.
            //::string logDir;
            auto logDir = ::remoting::ViewerConfig::getInstance()->getLogDir();
            auto logFileName = logDir / (::string(LogNames::VIEWER_LOG_FILE_STUB_NAME) + ".log");

            auto  logFile=file_item(logFileName);
            if (logFile->exists()) {
                m_openLogDir.enableWindow(true);
            } else {
                m_openLogDir.enableWindow(false);
            }
        } else {
            m_logging.enableWindow(false);
            m_openLogDir.enableWindow(false);
        }
    }
    void ConfigurationDialog::onOpenFolderButtonClick()
    {
        //::string logDir;

        auto logDir = ::remoting::ViewerConfig::getInstance()->getLogDir();

        auto pathLog = logDir / (::string(LogNames::VIEWER_LOG_FILE_STUB_NAME) + ".log");

        ::string command;

        command.format("explorer /select,{}", pathLog);

        Process explorer(command);

        try {
            explorer.start();
        } catch (...) {
            // TODO: Place error notification here.
        }
    }

    bool ConfigurationDialog::onInitDialog()
    {
        subclassControlById(m_showToolBars, IDC_CSHOWTOOLBARS);
        subclassControlById(m_warnAtSwitching, IDC_CWARNATSW);
        subclassControlById(m_numberConn, IDC_ENUMCON);
        subclassControlById(m_snumConn, IDC_SNUMCON);
        subclassControlById(m_reverseConn, IDC_EREVCON);
        subclassControlById(m_sreverseConn, IDC_SREVCON);
        subclassControlById(m_verbLvl, IDC_EVERBLVL);
        subclassControlById(m_sverbLvl, IDC_SVERBLVL);
        subclassControlById(m_logging, IDC_ELOGGING);
        subclassControlById(m_openLogDir, IDC_OPEN_LOG_FOLDER_BUTTON);

        m_snumConn.setRange(0, 1024);
        m_snumConn.setBuddy(&m_numberConn);

        m_sreverseConn.setRange32(1, 65535);
        m_sreverseConn.setBuddy(&m_reverseConn);

        m_sverbLvl.setRange(0, 9);
        m_sverbLvl.setBuddy(&m_verbLvl);

        updateControlValues();

        return false;
    }

    void ConfigurationDialog::updateControlValues()
    {
        auto config = ::remoting::ViewerConfig::getInstance();

        ::string txt;

        txt.formatf("{}", config->getListenPort());
        m_reverseConn.setText(txt);

        txt.formatf("{}", config->getLogLevel());
        m_verbLvl.setText(txt);

        txt.formatf("{}", config->getHistoryLimit());
        m_numberConn.setText(txt);

        m_showToolBars.check(config->isToolbarShown());
        m_warnAtSwitching.check(config->isPromptOnFullscreenEnabled());

        ::string logFileName;
        logFileName.formatf("{}\\{}.log", config->getPathToLogFile(), LogNames::VIEWER_LOG_FILE_STUB_NAME);
        m_logging.setText(logFileName);
    }

    bool ConfigurationDialog::isInputValid()
    {
        if (!testNum(&m_reverseConn, main_subsystem()->string_table()->getString(IDS_CONFIGURATION_LISTEN_PORT))) {
            return false;
        }
        if (!testNum(&m_verbLvl, main_subsystem()->string_table()->getString(IDS_CONFIGURATION_LOG_LEVEL))) {
            return false;
        }
        if (!testNum(&m_numberConn, main_subsystem()->string_table()->getString(IDS_CONFIGURATION_HISTORY_LIMIT))) {
            return false;
        }
        return true;
    }

    bool ConfigurationDialog::testNum(TextBox *tb, const ::scoped_string & scopedstrTbName)
    {
        //::string text;
        auto text = tb->getText();

        if (StringParser::tryParseInt(text)) {
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

        text = m_reverseConn.getText();
        main_subsystem()->string_parser()->parseInt(text, &intVal);
        config->setListenPort(intVal);

        text = m_verbLvl.getText();
        main_subsystem()->string_parser()->parseInt(text, &intVal);
        config->setLogLevel((enum_trace_level)intVal);

        int oldLimit = config->getHistoryLimit();
        text=m_numberConn.getText();
        main_subsystem()->string_parser()->parseInt(text, &intVal);
        config->setHistoryLimit(intVal);

        if (config->getHistoryLimit() < oldLimit) {
            config->getConnectionHistory()->truncate();
        }

        config->showToolbar(m_showToolBars.isChecked());
        config->promptOnFullscreen(m_warnAtSwitching.isChecked());

        SettingsManager *sm = ViewerSettingsManager::getInstance();
        config->saveToStorage(sm);
    }


} // namespace remoting_remoting



