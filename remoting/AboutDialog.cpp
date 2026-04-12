// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "AboutDialog.h"
#include "subsystem_acme/node/Shell.h"
#include "subsystem_acme/node/VersionInfo.h"
//#include "remoting/remoting_common/win_system/Environment.h"
#include "subsystem_apex/Control.h"
#include "BuildTime.h"
#include "resource.h"
#include "acme/filesystem/filesystem/file_context.h"
#include "subsystem_apex/subsystem.h"
#include "remoting/remoting_common/remoting.h"
#include "subsystem_acme/node/SystemException.h"


namespace remoting_remoting
{


    AboutDialog::AboutDialog()
    //: BaseDialog(IDD_ABOUT_DIALOG)
    {
       initialize_dialog(IDD_ABOUT_DIALOG);
    }

    AboutDialog::~AboutDialog()
    {
    }


    // void AboutDialog::initialize_about_dialog()
    // {
    //
    //
    //
    // }

    void AboutDialog::onCloseButtonClick()
    {
        closeDialog(::subsystem_apex::e_control_id_cancel);
    }

    void AboutDialog::onOrderSupportButtonClock()
    {
        openUrl(main_subsystem()->string_table()->getString(IDS_URL_LICENSING_FVA));
    }

    void AboutDialog::onVisitSiteButtonClick()
    {
        openUrl(main_subsystem()->string_table()->getString(IDS_URL_PRODUCT_FVA));
    }

    void AboutDialog::openUrl(const ::scoped_string & scopedstrUrl)
    {
        try {
            main_subsystem()->shell()->open(scopedstrUrl, 0, 0);
        } catch (::subsystem::SystemException &sysEx) {
            ::string strMessage;

            strMessage.formatf(main_subsystem()->string_table()->getString(IDS_FAILED_TO_OPEN_URL_FORMAT).c_str(), sysEx.get_message());

            main_subsystem()->message_box(operating_system_window(),
                       wstring(strMessage),
                       wstring(main_subsystem()->string_table()->getString(IDS_MBC_TVNVIEWER)),
                       ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);
        }
    }

    bool AboutDialog::onInitDialog()
    {
        // Update product version string.
        ::string versionString("unknown");
        try {
            ::string binaryPath;
            binaryPath = ::system()->file()->module();
            ::subsystem::VersionInfo productInfo;
           productInfo.initialize_version_info(binaryPath);
            versionString= productInfo.getProductVersionString();
        } catch (::subsystem::SystemException &ex) {
            main_subsystem()->message_box(operating_system_window(),
                       ::wstring(ex.get_message()),
                       ::wstring(main_subsystem()->string_table()->getString(IDS_MBC_TVNVIEWER)),
                       ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);
        }

        // Format product version and build time for displaying on the dialog.
        ::string versionText;
        versionText.formatf(main_subsystem()->string_table()->getString(IDS_PRODUCT_VERSION_FORMAT).c_str(),
                           ::string(versionString).c_str(),
                           BuildTime::DATE);

        // Show version info on the dialog.
        auto plabelVersion = dialog_item< ::subsystem_apex::Control >(IDC_STATIC_VERSION);
       plabelVersion->setText(versionText);
        //versionLabel.setWindow(GetDlgItem(m_hwnd, IDC_STATIC_VERSION));
        //versionLabel.setText(versionText);


       ::string strLicensingInfo = main_subsystem()->string_table()->getString(IDS_LICENSING_INFO);

        // Show licensing info and/or special build info.
       auto plabelLicensing = dialog_item< ::subsystem_apex::Control >(IDC_STATIC_VERSION);
        //::subsystem_apex::Control licensingLabel;
        //licensingLabel.setWindow(GetDlgItem(m_hwnd, IDC_STATIC_LICENSING));
        plabelLicensing->setText(strLicensingInfo);

        return false;
    }

    bool AboutDialog::onNotify(unsigned int controlID, ::lparam data)
    {
        return false;
    }

    bool AboutDialog::onCommand(unsigned int controlID, unsigned int notificationID)
    {
        switch (controlID) {
            case ::subsystem_apex::e_control_id_cancel:
                onCloseButtonClick();
                break;
            case IDC_ORDER_SUPPORT_BUTTON:
                onOrderSupportButtonClock();
                break;
            case IDC_VISIT_WEB_SITE_BUTTON:
                onVisitSiteButtonClick();
                break;
        }
        return false;
    }

    bool AboutDialog::onDestroy()
    {
        return false;
    }



} // namespace remoting_remoting



