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
#include "framework.h"
#include "remoting/rfb_node_desktop/resource.h"
#include "ServerConfigDialog.h"
#include "ConfigDialog.h"
#include "ChangePasswordDialog.h"
#include "acme/constant/user_notification.h"
#include "remoting/remoting_rfb/node_config/Configurator.h"
#include "subsystem/platform/StringParser.h"
#include "CommonInputValidation.h"
#include "UIDataAccess.h"
// #include aaa_<limits.h>
namespace remoting_rfb_node
{
   ServerConfigDialog::ServerConfigDialog(Configurator * pconfigurator)
   :  m_pdialogParent(NULL),
   m_pconfigurator(pconfigurator)
   {
      initialize_dialog(IDD_CONFIG_SERVER_PAGE);
   }

   ServerConfigDialog::~ServerConfigDialog()
   {
   }

   void ServerConfigDialog::setParentDialog(::innate_subsystem::DialogInterface *pdialog)
   {
      m_pdialogParent = pdialog;
   }

   bool ServerConfigDialog::onInitDialog()
   {
      m_config = m_pconfigurator->getServerConfig();
      initControls();
      updateUI();

      return true;
   }

   // bool ServerConfigDialog::onNotify(::u32 controlID, ::lparam data)
   // {
   //    if (controlID == IDC_POLLING_INTERVAL_SPIN) {
   //       LPNMUPDOWN scopedstrMessage = (LPNMUPDOWN)data;
   //       if (scopedstrMessage->hdr.code == UDN_DELTAPOS) {
   //          onPollingIntervalSpinChangePos(scopedstrMessage);
   //       }
   //    }
   //    return true;
   // }


   bool ServerConfigDialog::_002OnUpDown(int iControl, int iPos, int iDelta)
   {

      if (iControl == IDC_POLLING_INTERVAL_SPIN)
      {

         onPollingIntervalSpinChangePos(iControl, iPos, iDelta);

         return true;
      }

      return false;

   }

   bool ServerConfigDialog::onCommand(::u32 controlID, ::u32 notificationID)
   {
      if (notificationID == ::user::e_notification_button_clicked) {
         switch (controlID) {
            case IDC_ACCEPT_RFB_CONNECTIONS:
               onAcceptRfbConnectionsClick();
               break;
            case IDC_ACCEPT_HTTP_CONNECTIONS:
               onAcceptHttpConnectionsClick();
               break;
            case IDC_USE_AUTHENTICATION:
               onAuthenticationClick();
               break;
            case IDC_PRIMARY_PASSWORD:
               onPrimaryPasswordChange();
               break;
            case IDC_VIEW_ONLY_PASSWORD:
               onReadOnlyPasswordChange();
               break;
            case IDC_UNSET_PRIMARY_PASSWORD_BUTTON:
               onUnsetPrimaryPasswordClick();
               break;
            case IDC_UNSET_READONLY_PASSWORD_BUTTON:
               onUnsetReadOnlyPasswordClick();
               break;
            case IDC_ENABLE_FILE_TRANSFERS:
               onFileTransferCheckBoxClick();
               break;
            case IDC_REMOVE_WALLPAPER:
               onRemoveWallpaperCheckBoxClick();
               break;
            case IDC_BLOCK_LOCAL_INPUT:
               onBlockLocalInputChanged();
               break;
            case IDC_BLOCK_REMOTE_INPUT:
               onBlockLocalInputChanged();
               break;
            case IDC_LOCAL_INPUT_PRIORITY:
               onLocalInputPriorityChanged();
               break;
            case IDC_USE_D3D:
               onUseD3DChanged();
               break;
            case IDC_USE_MIRROR_DRIVER:
               // FIXME: For high quality code is needed to use a function.
            {
               auto pconfigdialog = m_pdialogParent->get_callback < ConfigDialog >();
               pconfigdialog->updateApplyButtonState();
            }
               break;
            case IDC_SHOW_TVNCONTROL_ICON_CHECKBOX:
               onShowTrayIconCheckBoxClick();
               break;
            case IDC_CONNECT_RDP_SESSION:
               onConnectToRdpCheckBoxClick();
               break;
         }
      } else if (notificationID == ::user::e_notification_edit_update) {
         switch (controlID) {
            case IDC_RFB_PORT:
               onRfbPortUpdate();
               break;
            case IDC_HTTP_PORT:
               onHttpPortUpdate();
               break;
            case IDC_POLLING_INTERVAL:
               onPollingIntervalUpdate();
               break;
            case IDC_LOCAL_INPUT_PRIORITY_TIMEOUT:
               onInactivityTimeoutUpdate();
               break;
         }
      }
      return true;
   }

   bool ServerConfigDialog::validateInput()
   {
      bool commonValidationOk =
        CommonInputValidation::validatePort(&m_rfbPort) &&
        CommonInputValidation::validatePort(&m_httpPort) &&
        CommonInputValidation::validateUINT(
          &m_pollingInterval,
          MainSubsystem().StringTable().getString(IDS_INVALID_POLLING_INTERVAL)) &&
        CommonInputValidation::validateUINT(
          &m_localInputPriorityTimeout,
          MainSubsystem().StringTable().getString(IDS_INVALID_INACTIVITY_TIMEOUT));

      if (!commonValidationOk) {
         return false;
      }

      int httpPort, rfbPort;

      UIDataAccess::queryValueAsInt(&m_rfbPort, &rfbPort);
      UIDataAccess::queryValueAsInt(&m_httpPort, &httpPort);

      if (rfbPort == httpPort && m_acceptHttpConnections.isChecked()) {
         CommonInputValidation::notifyValidationError(
           &m_httpPort,
           MainSubsystem().StringTable().getString(IDS_HTTP_RFB_PORTS_ARE_EQUAL));
         return false;
      }

      ::u32 pollingInterval;

      UIDataAccess::queryValueAsUInt(&m_pollingInterval, &pollingInterval);

      if (pollingInterval < ServerConfig::MINIMAL_POLLING_INTERVAL) {
         CommonInputValidation::notifyValidationError(
           &m_pollingInterval,
           MainSubsystem().StringTable().getString(IDS_POLL_INTERVAL_TOO_SMALL));
         return false;
      }

      ::u32 inactivityTimeout;

      UIDataAccess::queryValueAsUInt(&m_localInputPriorityTimeout, &inactivityTimeout);

      if (inactivityTimeout < ServerConfig::MINIMAL_LOCAL_INPUT_PRIORITY_TIMEOUT) {
         CommonInputValidation::notifyValidationError(
           &m_localInputPriorityTimeout,
           MainSubsystem().StringTable().getString(IDS_INACTIVITY_TIMEOUT_TOO_SMALL));
         return false;
      }

      bool passwordSpecified = m_ppControl->getState() == PasswordControl::OldPassword ||
                               m_ppControl->getState() == PasswordControl::NewPassword ||
                               m_vpControl->getState() == PasswordControl::OldPassword ||
                               m_vpControl->getState() == PasswordControl::NewPassword;

      if (m_acceptRfbConnections.isChecked() &&
          m_useAuthentication.isChecked() &&
          !passwordSpecified) {
         MainSubsystem().message_box(operating_system_window(),
                    MainSubsystem().StringTable().getString(IDS_SET_PASSWORD_NOTIFICATION),
                    MainSubsystem().StringTable().getString(IDS_CAPTION_BAD_INPUT),
                                     ::user::e_message_box_icon_stop | ::user::e_message_box_ok);
         return false;
          }

      return true;
   }

   void ServerConfigDialog::updateUI()
   {
      m_rfbPort.setSignedInt(m_config->getRfbPort());
      m_httpPort.setSignedInt(m_config->getHttpPort());
      m_pollingInterval.setUnsignedInt(m_config->getPollingInterval());

      m_enableFileTransfers.setChecked(m_config->isFileTransfersEnabled());
      m_removeWallpaper.setChecked(m_config->isRemovingDesktopWallpaperEnabled());

      m_acceptRfbConnections.setChecked(m_config->isAcceptingRfbConnections());
      m_acceptHttpConnections.setChecked(m_config->isAcceptingHttpConnections());

      m_ppControl->setHadPassword(m_config->hasPrimaryPassword());
      m_vpControl->setHadPassword(m_config->hasReadOnlyPassword());

      m_useAuthentication.setChecked(m_config->isUsingAuthentication());

      m_blockLocalInput.setChecked(m_config->isBlockingLocalInput());
      m_blockRemoteInput.setChecked(m_config->isBlockingRemoteInput());
      m_localInputPriority.setChecked(m_config->isLocalInputPriorityEnabled());
      if (m_config->isLocalInputPriorityEnabled()) {
         m_localInputPriorityTimeout.enableWindow(true);
      }
      m_localInputPriorityTimeout.setUnsignedInt(m_config->getLocalInputPriorityTimeout());

      m_useD3D.setChecked(m_config->getD3DIsAllowed());
      m_useMirrorDriver.setChecked(m_config->getMirrorIsAllowed());

      m_showTrayIcon.setChecked(m_config->getShowTrayIconFlag());
      m_connectToRdp.setChecked(m_config->getConnectToRdpFlag());

      updateCheckboxesState();
      updateControlDependencies();
   }

   void ServerConfigDialog::apply()
   {
      ::string rfbPortText;
      ::string httpPortText;
      // Polling interval string storage
      ::string pollingIntervalText;

      rfbPortText = m_rfbPort.getText();
      httpPortText = m_httpPort.getText();
      pollingIntervalText = m_pollingInterval.getText();

      int intVal = 0;

      MainSubsystem().StringParser().parseInt(rfbPortText, &intVal);
      m_config->setRfbPort(intVal);

      MainSubsystem().StringParser().parseInt(httpPortText, &intVal);
      m_config->setHttpPort(intVal);

      MainSubsystem().StringParser().parseInt(pollingIntervalText, &intVal);
      m_config->setPollingInterval(intVal);

      m_config->enableFileTransfers(m_enableFileTransfers.isChecked());
      m_config->enableRemovingDesktopWallpaper(m_removeWallpaper.isChecked());

      m_config->acceptRfbConnections(m_acceptRfbConnections.isChecked());
      m_config->acceptHttpConnections(m_acceptHttpConnections.isChecked());
      m_config->useAuthentication(m_useAuthentication.isChecked());

      //
      // Primary password.
      //

      if (m_ppControl->getState() == PasswordControl::NewPassword) {
         m_config->setPrimaryPassword((const unsigned char *)m_ppControl->getCryptedPassword());
      }
      if (m_ppControl->getState() == PasswordControl::ResetPassword) {
         m_config->deletePrimaryPassword();
      }

      //
      // View only password.
      //

      if (m_vpControl->getState() == PasswordControl::NewPassword) {
         m_config->setReadOnlyPassword((const unsigned char *)m_vpControl->getCryptedPassword());
      }
      if (m_vpControl->getState() == PasswordControl::ResetPassword) {
         m_config->deleteReadOnlyPassword();
      }

      // Local input priority timeout string storage
      ::string liptStringStorage;
      liptStringStorage = m_localInputPriorityTimeout.getText();
      int timeout = 0;

      m_config->setLocalInputPriority(m_localInputPriority.isChecked());
      if (MainSubsystem().StringParser().parseInt(liptStringStorage, &timeout)) {
         timeout = maximum(0, timeout);
         m_config->setLocalInputPriorityTimeout((::u32)timeout);
      }
      m_config->blockLocalInput(m_blockLocalInput.isChecked());
      m_config->blockRemoteInput(m_blockRemoteInput.isChecked());

      m_config->setMirrorAllowing(m_useMirrorDriver.isChecked());
      m_config->setD3DAllowing(m_useD3D.isChecked());
      m_config->setShowTrayIconFlag(m_showTrayIcon.isChecked());
      m_config->setConnectToRdpFlag(m_connectToRdp.isChecked());
   }

   void ServerConfigDialog::initControls()
   {
      //HWND hwnd = operating_system_window();
      dialog_item(m_rfbPort, IDC_RFB_PORT);
      dialog_item(m_httpPort, IDC_HTTP_PORT);
      dialog_item(m_pollingInterval, IDC_POLLING_INTERVAL);
      dialog_item(m_useD3D, IDC_USE_D3D);
      dialog_item(m_useMirrorDriver, IDC_USE_MIRROR_DRIVER);
      dialog_item(m_enableFileTransfers, IDC_ENABLE_FILE_TRANSFERS);
      dialog_item(m_removeWallpaper, IDC_REMOVE_WALLPAPER);
      dialog_item(m_acceptRfbConnections, IDC_ACCEPT_RFB_CONNECTIONS);
      dialog_item(m_acceptHttpConnections, IDC_ACCEPT_HTTP_CONNECTIONS);
      dialog_item(m_primaryPassword, IDC_PRIMARY_PASSWORD);
      dialog_item(m_readOnlyPassword, IDC_VIEW_ONLY_PASSWORD);
      dialog_item(m_useAuthentication, IDC_USE_AUTHENTICATION);
      dialog_item(m_unsetPrimaryPassword, IDC_UNSET_PRIMARY_PASSWORD_BUTTON);
      dialog_item(m_unsetReadOnlyPassword, IDC_UNSET_READONLY_PASSWORD_BUTTON);
      dialog_item(m_showTrayIcon, IDC_SHOW_TVNCONTROL_ICON_CHECKBOX);
      dialog_item(m_connectToRdp, IDC_CONNECT_RDP_SESSION);

      dialog_item(m_rfbPortSpin, IDC_RFB_PORT_SPIN);
      dialog_item(m_httpPortSpin, IDC_HTTP_PORT_SPIN);
      dialog_item(m_pollingIntervalSpin, IDC_POLLING_INTERVAL_SPIN);

      m_rfbPortSpin.setBuddy(&m_rfbPort);
      m_rfbPortSpin.setAccel(0, 1);
      m_rfbPortSpin.setRange32(1, 65535);

      m_httpPortSpin.setBuddy(&m_httpPort);
      m_httpPortSpin.setAccel(0, 1);
      m_httpPortSpin.setRange32(1, 65535);

      ::array_base<int> limitters{50, 200};
      ::array_base<int> deltas{5, 10};

      m_pollingIntervalSpin.setBuddy(&m_pollingInterval);
      m_pollingIntervalSpin.setAccel(0, 1);
      m_pollingIntervalSpin.setRange32(1, INT_MAX);
      m_pollingIntervalSpin.setAutoAccelerationParams(limitters, deltas, 50);
      m_pollingIntervalSpin.enableAutoAcceleration(true);

      dialog_item(m_blockLocalInput, IDC_BLOCK_LOCAL_INPUT);
      dialog_item(m_blockRemoteInput, IDC_BLOCK_REMOTE_INPUT);
      dialog_item(m_localInputPriority, IDC_LOCAL_INPUT_PRIORITY);
      dialog_item(m_localInputPriorityTimeout, IDC_LOCAL_INPUT_PRIORITY_TIMEOUT);
      dialog_item(m_inactivityTimeoutSpin, IDC_INACTIVITY_TIMEOUT_SPIN);

      m_inactivityTimeoutSpin.setBuddy(&m_localInputPriorityTimeout);
      m_inactivityTimeoutSpin.setAccel(0, 1);
      m_inactivityTimeoutSpin.setRange32(0, INT_MAX);

      m_ppControl = allocateø PasswordControl(&m_primaryPassword, &m_unsetPrimaryPassword);
      m_vpControl = allocateø PasswordControl(&m_readOnlyPassword, &m_unsetReadOnlyPassword);
   }

   //
   // TODO: Add comment to this method
   //

   void ServerConfigDialog::updateControlDependencies()
   {
      if (m_acceptRfbConnections.isChecked()) {
         m_rfbPort.enableWindow(true);
         m_acceptHttpConnections.enableWindow(true);
         m_useAuthentication.enableWindow(true);
      } else {
         m_rfbPort.enableWindow(false);
         m_acceptHttpConnections.enableWindow(false);
         m_useAuthentication.enableWindow(false);
      }

      if ((m_acceptHttpConnections.isChecked()) && (m_acceptHttpConnections.isEnabled())) {
         m_httpPort.enableWindow(true);
      } else {
         m_httpPort.enableWindow(false);
      }

      bool passwordsAreEnabled = ((m_useAuthentication.isChecked()) && (m_useAuthentication.isEnabled()));

      m_ppControl->enableWindow(passwordsAreEnabled);
      m_vpControl->enableWindow(passwordsAreEnabled);

      m_rfbPortSpin.invalidate();
      m_httpPortSpin.invalidate();
      m_pollingIntervalSpin.invalidate();
   }

   void ServerConfigDialog::onAcceptRfbConnectionsClick()
   {
      updateControlDependencies();
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onAcceptHttpConnectionsClick()
   {
      updateControlDependencies();
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onAuthenticationClick()
   {
      updateControlDependencies();
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }


   void ServerConfigDialog::onShowTrayIconCheckBoxClick()
   {
      bool oldVal = m_config->getShowTrayIconFlag();
      bool newVal = m_showTrayIcon.isChecked();

      if (oldVal != newVal) {
         auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
         pconfigdialog->updateApplyButtonState();
      }
   }

   void ServerConfigDialog::onConnectToRdpCheckBoxClick()
   {
      bool oldVal = m_config->getConnectToRdpFlag();
      bool newVal = m_connectToRdp.isChecked();

      if (oldVal != newVal) {
         auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
         pconfigdialog->updateApplyButtonState();
      }
   }



   void ServerConfigDialog::onPrimaryPasswordChange()
   {
      if (m_ppControl->showChangePasswordModalDialog(this)) {
         auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
         pconfigdialog->updateApplyButtonState();
      }
   }

   void ServerConfigDialog::onReadOnlyPasswordChange()
   {
      if (m_vpControl->showChangePasswordModalDialog(this)) {
         auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
         pconfigdialog->updateApplyButtonState();
      }
   }

   void ServerConfigDialog::onUnsetPrimaryPasswordClick()
   {
      m_ppControl->unsetPassword(true, operating_system_window());
auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onUnsetReadOnlyPasswordClick()
   {
      m_vpControl->unsetPassword(true, operating_system_window());
auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onPollingIntervalSpinChangePos(int iControl, int iPos, int iDelta)
   {
      if (iControl == IDC_POLLING_INTERVAL_SPIN)
      {
         m_pollingIntervalSpin.autoAccelerationHandler(iPos, iDelta);
      }
   }

   void ServerConfigDialog::onRfbPortUpdate()
   {auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onHttpPortUpdate()
   {auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onUrlParamsClick()
   {auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onPollingIntervalUpdate()
   {auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onFileTransferCheckBoxClick()
   {auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onRemoveWallpaperCheckBoxClick()
   {auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onGrabTransparentWindowsChanged()
   {auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onUseD3DChanged()
   {auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onBlockLocalInputChanged()
   {
      updateCheckboxesState();
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onBlockRemoteInputChanged()
   {
      updateCheckboxesState();
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onLocalInputPriorityChanged()
   {
      updateCheckboxesState();
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::onInactivityTimeoutUpdate()
   {
      auto pconfigdialog = m_pdialogParent->get_callback<ConfigDialog>();
      pconfigdialog->updateApplyButtonState();
   }

   void ServerConfigDialog::updateCheckboxesState()
   {
      if (m_blockLocalInput.isChecked() || m_blockRemoteInput.isChecked()) {
         m_localInputPriority.setChecked(false);
         m_localInputPriority.enableWindow(false);
      } else {
         m_localInputPriority.enableWindow(true);
      }

      if (m_localInputPriority.isChecked() && m_localInputPriority.isEnabled()) {
         m_localInputPriorityTimeout.enableWindow(true);
      } else {
         m_localInputPriorityTimeout.enableWindow(false);
      }
      m_inactivityTimeoutSpin.invalidate();
   }
} // namespace remoting_rfb_node



