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
#include "CommonInputValidation.h"

#include "subsystem/platform/StringParser.h"

#include "innate_subsystem/gui/BalloonTip.h"

#include "remoting/rfb_node_desktop/resource.h"

#include "UIDataAccess.h"
namespace remoting_rfb_node
{
   CommonInputValidation::CommonInputValidation()
   {
   }

   bool CommonInputValidation::validatePort(::innate_subsystem::TextBox *textBox)
   {
      bool validationOk = true;

      ::i32 parsedPortValue = 0;

      if (!UIDataAccess::queryValueAsInt(textBox, &parsedPortValue)) {
         validationOk = false;
      } else {
         if (parsedPortValue <= 0 || parsedPortValue > 65535) {
            validationOk = false;
         }
      }

      if (!validationOk) {
         notifyValidationError(textBox, MainSubsystem().StringTable().getString(IDS_PORT_RANGE_ERROR));
      }

      return validationOk;
   }

   bool CommonInputValidation::validateUINT(::innate_subsystem::TextBox *textBox, const ::scoped_string & scopedstrMessageText)
   {
      bool validationOk = true;

      ::u32 parsedValue = 0;

      if (!UIDataAccess::queryValueAsUInt(textBox, &parsedValue)) {
         validationOk = false;
      }

      if (!validationOk) {
         notifyValidationError(textBox, scopedstrMessageText);
      }

      return validationOk;
   }

   void CommonInputValidation::notifyValidationError(::innate_subsystem::TextBox *textBox, const ::scoped_string & scopedstrMessageText)
   {
      ::innate_subsystem::Tooltip invalidInputTip;

      invalidInputTip.setIconType(::innate_subsystem::e_tooltip_icon_warning);
      invalidInputTip.setText(scopedstrMessageText);
      invalidInputTip.setTitle(MainSubsystem().StringTable().getString(IDS_CAPTION_BAD_INPUT));

      textBox->showBalloonTip(&invalidInputTip);
      textBox->setFocus();
   }
} //namespace remoting_rfb_node




