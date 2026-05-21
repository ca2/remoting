// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "framework.h"
#include "AboutDialog.h"

#include "resource.h"

#include "subsystem/node/Shell.h"
#include "subsystem/node/VersionInfo.h"
#include "subsystem/node/OperatingSystem.h"

#include "innate_subsystem/gui/Control.h"

#include "remoting/node_desktop/BuildTime.h"
#include "subsystem/node/SystemException.h"


namespace remoting_control_desktop
{
   AboutDialog::AboutDialog()
   //: Dialog(IDD_ABOUT_DIALOG)
   {
      initialize_dialog(IDD_ABOUT_DIALOG);
   }

   AboutDialog::~AboutDialog()
   {
   }

   void AboutDialog::onCloseButtonClick()
   {
      closeDialog(::innate_subsystem::e_control_id_cancel);
   }

   void AboutDialog::onOrderSupportButtonClock()
   {
      openUrl(MainSubsystem().StringTable().getString(IDS_URL_LICENSING_FSA));
   }

   void AboutDialog::onVisitSiteButtonClick()
   {
      openUrl(MainSubsystem().StringTable().getString(IDS_URL_PRODUCT_FSA));
   }

   void AboutDialog::openUrl(const ::scoped_string & scopedstrUrl)
   {
      try {
         MainSubsystem().Shell().open(scopedstrUrl, 0, 0);
      } catch (::subsystem::SystemException &sysEx) {
         ::string scopedstrMessage;

         scopedstrMessage.runtime_format(MainSubsystem().StringTable().getString(IDS_FAILED_TO_OPEN_URL_FORMAT), sysEx.get_message());

         MainSubsystem().message_box(this->operating_system_window(),
           scopedstrMessage,
           MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
           ::user::e_message_box_ok|::user::e_message_box_icon_exclamation);
      }
   }

   bool AboutDialog::onInitDialog()
   {
      // Update product version string.
      ::string versionString("unknown");
      try {
         ::string binaryPath;
         binaryPath = MainSubsystem().OperatingSystem().getCurrentModulePath();;
         ::subsystem::VersionInfo productInfo;
         productInfo.initialize_version_info(binaryPath);
         versionString= productInfo.getProductVersionString();
      } catch (::subsystem::SystemException &ex) {
         MainSubsystem().message_box(this->operating_system_window(),
                    ex.get_message(),
                    MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
                    ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);
      }

      // Format product version and build time for displaying on the dialog.
      ::string versionText;
      versionText.runtime_format(MainSubsystem().StringTable().getString(IDS_PRODUCT_VERSION_FORMAT),
                         versionString,
                         BuildTime::DATE);

      // Show version info on the dialog.
      ::innate_subsystem::Control versionLabel;
      dialog_item(versionLabel, IDC_STATIC_VERSION);
      versionLabel.setText(versionText);

      // Show licensing info and/or special build info.
      ::innate_subsystem::Control licensingLabel;
      dialog_item(licensingLabel, IDC_STATIC_LICENSING);
      licensingLabel.setText(MainSubsystem().StringTable().getString(IDS_LICENSING_INFO));

      return false;
   }

   bool AboutDialog::onNotify(::u32 controlID, ::lparam data)
   {
      return false;
   }

   bool AboutDialog::onCommand(::u32 controlID, ::u32 notificationID)
   {
      switch (controlID) {
         case ::innate_subsystem::e_control_id_cancel:
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
} // namespace remoting_control_desktop

