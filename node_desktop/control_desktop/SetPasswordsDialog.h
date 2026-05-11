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

#pragma once


//#include "subsystem/platform/winhdr.h"
//#include "acme/_operating_system.h"


#include "innate_subsystem/gui/Dialog.h"
#include "innate_subsystem/gui/CheckBox.h"
#include "innate_subsystem/gui/TextBox.h"
#include "innate_subsystem/gui/Tooltip.h"

#include "subsystem/platform/RegistryKey.h"

#include "remoting/remoting/config/ConnectionHistory.h"


namespace remoting_control_desktop
{

   // Set passwords dialog. Just allows user to set rfb and administrators
   // passwords.
   class SetPasswordsDialog : public ::innate_subsystem::Dialog
   {
   public:
      SetPasswordsDialog(bool initStateOfUseRfbAuth,
                         bool initStateOfUseAdminAuth);
      virtual ~SetPasswordsDialog();

      // Returns true if the checkbox has been checked
      bool getUseRfbPass();
      // Returns true if no rfb authentication is needed.
      bool getRfbPassForClear();
      void getRfbPass(::string & pass);

      // Returns true if the checkbox has been checked
      bool getUseAdmPass();
      // Returns true if no admin authentication is needed.
      bool getAdmPassForClear();
      void getAdmPass(::string & pass);

   protected:
      void initControls();

      virtual bool onInitDialog();
      virtual bool onNotify(::u32 controlID, ::lparam data);
      virtual bool onCommand(::u32 controlID, ::u32 notificationID);
      virtual bool onClose();
      virtual bool onDestroy();

      void onOkButtonClick();
      void onUseRfbAuthCheckBoxClick();
      void onProtectControlInterfaceCheckBoxClick();

   protected:
      // UPdates internal state by the radio buttons.
      void readRadio();
      // Enables and disables the edit controls by the current state.
      void updateEditControls();



      ::innate_subsystem::CheckBox m_dontChangeRfbAuthSettingsRadio;
      ::innate_subsystem::CheckBox m_dontUseRfbAuthRadio;
      ::innate_subsystem::CheckBox m_useRfbAuthRadio;
      ::innate_subsystem::TextBox m_rfbPassEdit1;
      ::innate_subsystem::TextBox m_rfbPassEdit2;

      ::innate_subsystem::CheckBox m_dontChangeAdminAuthSettingsRadio;
      ::innate_subsystem::CheckBox m_dontUseAdminAuthRadio;
      ::innate_subsystem::CheckBox m_useAdminAuthRadio;
      ::innate_subsystem::TextBox m_admPassEdit1;
      ::innate_subsystem::TextBox m_admPassEdit2;

      // Tooltip
      ::innate_subsystem::Tooltip m_passwordEmptyTooltip;
      ::innate_subsystem::Tooltip m_passwordsNotMatchTooltip;
      ::innate_subsystem::Tooltip m_passwordWeakTooltip;

      bool m_dontChangeRfbAuth;
      bool m_useRfbAuth;
      ::string m_rfbPass;

      bool m_dontChangeAdmAuth;
      bool m_protectControlInterface;
      ::string m_admPass;
   };


} // namespace remoting_control_desktop




