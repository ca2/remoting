// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "ConfigDialog.h"
#include "remoting/node_desktop/resource.h"
#include "subsystem/_common_header.h"
namespace remoting_node
{
   ConfigDialog::ConfigDialog(Configurator * pconfigurator, ::remoting_control_desktop::ControlCommand *reloadConfigCommand)
   :
   m_videoRegionsConfigDialog(pconfigurator),
   m_administrationConfigDialog(pconfigurator),
     m_isConfiguringService(pconfigurator->m_isConfiguringService),
     m_reloadConfigCommand(reloadConfigCommand),
     m_lastSelectedTabIndex(0)
   {
      initialize_dialog(IDD_CONFIG);
   }

   ConfigDialog::ConfigDialog(Configurator * pconfigurator)
   : //BaseDialog(IDD_CONFIG),
   m_videoRegionsConfigDialog(pconfigurator),
   m_administrationConfigDialog(pconfigurator),
     m_isConfiguringService(pconfigurator->m_isConfiguringService),
     m_reloadConfigCommand(NULL),
     m_lastSelectedTabIndex(0)
   {
      initialize_dialog(IDD_CONFIG);
   }

   ConfigDialog::ConfigDialog()
   : //BaseDialog(IDD_CONFIG),
   m_videoRegionsConfigDialog(nullptr),
   m_administrationConfigDialog(nullptr),
     m_isConfiguringService(false),
     m_reloadConfigCommand(NULL),
     m_lastSelectedTabIndex(0)
   {
      initialize_dialog(IDD_CONFIG);
   }

   ConfigDialog::~ConfigDialog()
   {
   }

   // FIXME: Unimplemented
   void ConfigDialog::updateApplyButtonState()
   {
      m_ctrlApplyButton.enableWindow(true);
   }

   void ConfigDialog::setConfigReloadCommand(::remoting_control_desktop::ControlCommand *command)
   {
      m_reloadConfigCommand = command;

      updateCaption();
   }

   void ConfigDialog::setServiceFlag(bool serviceFlag)
   {
      m_isConfiguringService = serviceFlag;

      updateCaption();
   }

   bool ConfigDialog::isConfiguringService()
   {
      return m_isConfiguringService;
   }

   void ConfigDialog::initControls()
   {
      //HWND dialogHwnd = m_ctrlThis.operating_system_window();

      dialog_item(m_ctrlApplyButton, IDC_APPLY);
      dialog_item(m_tabControl, IDC_CONFIG_TAB);

      //
      // Change caption of dialog
      //

      updateCaption();
   }

   void ConfigDialog::loadSettings()
   {
      m_pconfigurator->load();
   }

   bool ConfigDialog::onCommand(unsigned int controlID, unsigned int notificationID)
   {
      switch (controlID) {
         case ::innate_subsystem::e_control_id_ok:
            onOKButtonClick();
            break;
         case ::innate_subsystem::e_control_id_cancel:
            onCancelButtonClick();
            break;
         case IDC_APPLY:
            onApplyButtonClick();
            break;
      }
      return true;
   }

   // bool ConfigDialog::onNotify(unsigned int controlID, ::lparam data)
   // {
   //    switch (controlID) {
   //       case IDC_CONFIG_TAB:
   //          switch (((LPNMHDR)data)->code) {
   //          case TCN_SELCHANGE:
   //                onTabChange();
   //                break;
   //          case TCN_SELCHANGING:
   //                onTabChanging();
   //                break;
   //          }
   //          break;
   //    }
   //    return true;
   // }

   bool ConfigDialog::onInitDialog()
   {
      //m_config = m_pconfigurator;
      m_pconfigurator->setServiceFlag(m_isConfiguringService);

      initControls();

      m_tabControl.addTab(NULL, "Temp");

      auto pimpl = impl < ::

      m_serverConfigDialog.setParent();
      m_serverConfigDialog.setParentDialog(this);
      m_serverConfigDialog.create();
      moveDialogToTabControl(&m_serverConfigDialog);

      m_portMappingDialog.setParent(&m_ctrlThis);
      m_portMappingDialog.setParentDialog(this);
      m_portMappingDialog.create();
      moveDialogToTabControl(&m_portMappingDialog);
      m_portMappingDialog.hide();

      m_administrationConfigDialog.setParent(&m_ctrlThis);
      m_administrationConfigDialog.setParentDialog(this);
      m_administrationConfigDialog.create();
      moveDialogToTabControl(&m_administrationConfigDialog);

      m_ipAccessControlDialog.setParent(&m_ctrlThis);
      m_ipAccessControlDialog.setParentDialog(this);
      m_ipAccessControlDialog.create();
      moveDialogToTabControl(&m_ipAccessControlDialog);

      m_videoRegionsConfigDialog.setParent(&m_ctrlThis);
      m_videoRegionsConfigDialog.setParentDialog(this);
      m_videoRegionsConfigDialog.create();
      moveDialogToTabControl(&m_videoRegionsConfigDialog);

      m_tabControl.addTab(&m_serverConfigDialog, MainSubsystem().StringTable().getString(IDS_SERVER_TAB_CAPTION));
      m_tabControl.addTab(&m_portMappingDialog, MainSubsystem().StringTable().getString(IDS_EXTRA_PORTS_TAB_CAPTION));
      m_tabControl.addTab(&m_ipAccessControlDialog, MainSubsystem().StringTable().getString(IDS_ACCESS_CONTROL_TAB_CAPTION));
      m_tabControl.addTab(&m_videoRegionsConfigDialog, MainSubsystem().StringTable().getString(IDS_VIDEO_WINDOWS_TAB_CAPTION));
      m_tabControl.addTab(&m_administrationConfigDialog, MainSubsystem().StringTable().getString(IDS_ADMINISTRATION_TAB_CAPTION));

      m_tabControl.removeTab(0);

      m_tabControl.showTab(m_lastSelectedTabIndex);
      m_tabControl.setFocus();

      m_ctrlApplyButton.enableWindow(false);
      m_ctrlThis.set_foreground_window();

      return false;
   }

   bool ConfigDialog::onDestroy()
   {
      m_lastSelectedTabIndex = m_tabControl.getSelectedTabIndex();
      m_tabControl.deleteAllTabs();
      return true;
   }

   void ConfigDialog::onCancelButtonClick()
   {
      kill(0);
   }

   void ConfigDialog::onOKButtonClick()
   {
      onApplyButtonClick();
      if (!m_ctrlApplyButton.isEnabled()) { // onApplyButtonClick() has been successfully processed.
         kill(0);
      }
   }

   void ConfigDialog::onApplyButtonClick()
   {
      // Check values that specified in gui.
      bool canApply = m_ctrlApplyButton.isEnabled() && validateInput();

      // Fill global server configuration with values from gui.
      if (canApply) {
         m_administrationConfigDialog.apply();
         m_serverConfigDialog.apply();
         m_ipAccessControlDialog.apply();
         m_videoRegionsConfigDialog.apply();
      } else {
         return ;
      }

      // If reload command is specified then we're working in online mode
      // and we don't have to save configuration locally.
      if (m_reloadConfigCommand != NULL) {
         m_reloadConfigCommand->execute();

         if (m_reloadConfigCommand->executionResultOk()) {
            m_administrationConfigDialog.updateUI();
            m_ipAccessControlDialog.updateUI();
            m_ctrlApplyButton.enableWindow(false);
         }
         return;
      }
      // We're working in offline mode and we need to save config
      if (!m_config->save()) {
         MainSubsystem().message_box(m_ctrlThis.operating_system_window(),
                    MainSubsystem().StringTable().getString(IDS_CANNOT_SAVE_CONFIG),
                    MainSubsystem().StringTable().getString(IDS_MBC_ERROR),
                    ::user::e_message_box_ok | MB_ICONERROR);
         return;
      }
      m_ctrlApplyButton.enableWindow(false);
      MainSubsystem().message_box(m_ctrlThis.operating_system_window(),
        MainSubsystem().StringTable().getString(IDS_OFFLINE_CONFIG_SAVE_NOTIFICATION),
        MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
        ::user::e_message_box_ok | MB_ICONINFORMATION);
   }

   void ConfigDialog::onTabChange()
   {
      int currentTabIndex = m_tabControl.getSelectedTabIndex();
      Tab *tab = m_tabControl.getTab(currentTabIndex);
      tab->set_visible(true);
   }

   void ConfigDialog::onTabChanging()
   {
      int currentTabIndex = m_tabControl.getSelectedTabIndex();
      Tab *tab = m_tabControl.getTab(currentTabIndex);
      tab->set_visible(false);
   }

   void ConfigDialog::moveDialogToTabControl(BaseDialog *dialog)
   {
      RECT rectangle;
      POINT first, last;

      m_tabControl.adjustRect(rectangle);

      first.x = rectangle.left;
      first.y = rectangle.top;
      last.x = rectangle.right;
      last.y = rectangle.bottom;

      HWND hwndFrom = m_tabControl.operating_system_window();
      HWND hwndTo = dialog->operating_system_window();

      MapWindowPoints(hwndFrom, hwndTo, &first, 1);
      MapWindowPoints(hwndFrom, hwndTo, &last, 1);

      MoveWindow(dialog->operating_system_window(),
                 first.x, first.y, last.x - first.x, last.y - first.y, true);
   }

   bool ConfigDialog::validateInput()
   {
      if (!m_serverConfigDialog.validateInput()) {
         m_tabControl.showTab(&m_serverConfigDialog);
         return false;
      }
      if (!m_ipAccessControlDialog.validateInput()) {
         m_tabControl.showTab(&m_ipAccessControlDialog);
         return false;
      }
#ifdef USE_EXTRA_TABS
      if (!m_videoRegionsConfigDialog.validateInput()) {
         m_tabControl.showTab(&m_videoRegionsConfigDialog);
         return false;
      }
      if (!m_administrationConfigDialog.validateInput()) {
         m_tabControl.showTab(&m_administrationConfigDialog);
         return false;
      }
#else
      if (!m_administrationConfigDialog.validateInput()) {
         m_tabControl.showTab(&m_administrationConfigDialog);
         return false;
      }
#endif
      return true;
   }

   void ConfigDialog::updateCaption()
   {
      ::string caption;

      caption.format(MainSubsystem().StringTable().getString(IDS_SERVER_CONFIG_CAPTION_FORMAT),
                     MainSubsystem().StringTable().getString(m_isConfiguringService ? IDS_SERVICE : IDS_SERVER),
                     m_reloadConfigCommand == 0 ? MainSubsystem().StringTable().getString(IDS_OFFLINE_MODE) : "");

      m_ctrlThis.setText(caption);
   }
} // namespace remoting_node



