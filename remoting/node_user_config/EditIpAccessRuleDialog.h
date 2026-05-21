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

#pragma once


#include "innate_subsystem/gui/Dialog.h"
#include "innate_subsystem/gui/TextBox.h"
#include "innate_subsystem/gui/CheckBox.h"
#include "innate_subsystem/gui/BalloonTip.h"

#include "remoting/remoting/node_config/IpAccessRule.h"

//////#include "subsystem/platform/::string.h"
///
namespace remoting_node
{
   class EditIpAccessRuleDialog : virtual public ::innate_subsystem::Dialog
   {
   public:
      EditIpAccessRuleDialog();
      virtual ~EditIpAccessRuleDialog();

      void setIpAccessControl(IpAccessRule *iac) { m_data = iac; }
      IpAccessRule *getIpAccessControl() { return m_data; }

      void setEditFlag(bool flagEnabled);

   protected:

      //
      // BaseDialog overrided methods
      //

      virtual bool onInitDialog() override;
      virtual bool onCommand(::u32 controlID, ::u32 notificationID) override;
      virtual bool onNotify(::u32 controlID, ::lparam data) { return true; }
      virtual bool onDestroy() override { return true; }

      //
      // Controls event handlers
      //

      void onOkButtonClick();
      void onCancelButtonClick();
      void onAccessTypeRadioClick(int num);
   private:
      void initControls();
      bool validateInput();

   protected:
      // Controls
      ::innate_subsystem::TextBox m_firstIp;
      ::innate_subsystem::TextBox m_lastIp;
      ::innate_subsystem::CheckBox m_access[3];
      // Data
      IpAccessRule *m_data;
      bool m_isOpenedForEdit;

      //
      // Balloon tips with warning when user writes incorrect value
      // to textbox
      //

      ::innate_subsystem::Tooltip m_warningBalloonTip;
      ::innate_subsystem::Tooltip m_lastIpLessThanFirstBT;
   };
} // namespace remoting_node






