// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "node_config/Configurator.h"

#include "remoting/remoting_rtc/node_config/PortMapping.h"

namespace remoting_rtc_node
{
   class EditPortMappingDialog : virtual public ::innate_subsystem::Dialog
   {
   public:
      typedef enum {
         Add   = 0x0,
         Edit  = 0x1
      } DialogType;

   public:
      EditPortMappingDialog(::remoting_rtc_node::Configurator * pconfigurator, DialogType dlgType);
      virtual ~EditPortMappingDialog();

   public:
      void setMapping(PortMapping *mapping);
   protected:
      void initControls();
      bool isUserDataValid();

      //
      // Inherited from BaseDialog
      //

      bool onInitDialog() override;
      bool onCommand(::u32 controlID, ::u32 notificationID) override;
      bool onNotify(::u32 controlID, ::lparam data) { return true; }
      bool onDestroy() override { return true; }

      void onOkButtonClick();
      void onCancelButtonClick();
   protected:
      ::pointer < ::remoting_rtc_node::Configurator > m_pconfigurator;
      ::innate_subsystem::TextBox m_geometryTextBox;
      ::innate_subsystem::TextBox m_portTextBox;
      DialogType m_dialogType;
      PortMapping *m_mapping;
   };
}//   namespace remoting_rtc_node



