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


#include "innate_subsystem/gui/Dialog.h"
#include "innate_subsystem/gui/TextBox.h"
#include "innate_subsystem/gui/BalloonTip.h"
//////#include "subsystem/platform/::string.h"
///
///
namespace remoting_node
{
   class ChangePasswordDialog : virtual public ::innate_subsystem::Dialog
   {
   public:
      /**
      Creates new modal dialog for changing password.
      @param parent - parent dialog, optional (can be null).
      @param isNewPassword - flag determinates caption of dialog.
      */
      ChangePasswordDialog(::innate_subsystem::Control *parent, bool isNewPassword);
      virtual ~ChangePasswordDialog();

      /**
      Returns password (in plain text) specified by user.
      @return password string.
      */
      ::string getPasswordInPlainText() const;
   protected:

      //
      // BaseDialog overrided methods
      //

      virtual bool onInitDialog();
      virtual bool onCommand(unsigned int cID, unsigned int nID);
      virtual bool onNotify(unsigned int controlID, ::lparam data) { return true; }
      virtual bool onDestroy() { return true; }

      //
      // Controls event handlers
      //

      void onOkButtonClick();
      void onCancelButtonClick();

   private:
      void initControls();

   protected:
      // Controls
      ::innate_subsystem::TextBox m_password1;
      ::innate_subsystem::TextBox m_password2;
      // Password
      ::string m_passwordText;
      // Tooltip
      ::innate_subsystem::Tooltip m_passwordEmptyTooltip;
      ::innate_subsystem::Tooltip m_passwordsNotMatchTooltip;
      ::innate_subsystem::Tooltip m_passwordWeakTooltip;
      // Helper members
      bool m_newPassword;
      bool m_allowEmptyPassword;
   };
} //    namespace remoting_node




