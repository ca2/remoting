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
#include "innate_subsystem/gui/Window.h"
#include "innate_subsystem/gui/ListBox.h"

#include "remoting/remoting/node_config/Configurator.h"

namespace remoting_node
{
   class PortMappingDialog : virtual public ::innate_subsystem::Dialog
   {
   public:
      PortMappingDialog(::remoting_node::Configurator * pconfigurator);
      virtual ~PortMappingDialog();

      void setParentDialog(::innate_subsystem::DialogInterface * pdialog);;

   protected:

      //
      // Inherited from BaseDialog.
      //

      bool onInitDialog() override;
      bool onCommand(::u32 controlID, ::u32 notificationID) override;
      bool onNotify(::u32 controlID, ::lparam data) { return true; }
      bool onDestroy() override { return true; }

      //
      // Controls event handlers.
      //

      void onAddButtonClick();
      void onEditButtonClick();
      void onRemoveButtonClick();
      void onExPortsListBoxSelChange();
      void onExPortsListBoxDoubleClick();

   private:
      void initControls();

   protected:
      ::pointer < Configurator > m_pconfigurator;
      ::innate_subsystem::ListBox m_exPortsListBox;
      ::innate_subsystem::Control m_editButton;
      ::innate_subsystem::Control m_removeButton;

      PortMappingContainer *m_extraPorts;

      ::pointer < ::innate_subsystem::DialogInterface > m_pdialogParent;
   };
} // namespace remoting_node





