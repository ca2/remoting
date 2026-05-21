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
#include "UIDataAccess.h"

#include "subsystem/platform/StringParser.h"

namespace remoting_node
{
   UIDataAccess::UIDataAccess()
   {
   }

   bool UIDataAccess::queryValueAsInt(::innate_subsystem::TextBox *textBox, int *value)
   {
      ASSERT(textBox != 0);

      ::string textBoxText;

      textBoxText = textBox->getText();

      return MainSubsystem().StringParser().parseInt(textBoxText, value);
   }

   bool UIDataAccess::queryValueAsUInt(::innate_subsystem::TextBox *textBox, ::u32 *value)
   {
      ASSERT(textBox != 0);

      ::string textBoxText;

      textBoxText = textBox->getText();

      return MainSubsystem().StringParser().parseUInt(textBoxText, value);
   }
} // namespace remoting_node



