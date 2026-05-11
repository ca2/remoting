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


namespace remoting_control_desktop
{
   /**
    * ::innate_subsystem::Control authentication dialog.
    */
   class ControlAuthDialog : public innate_subsystem::Dialog
   {
   public:
      ControlAuthDialog();
      virtual ~ControlAuthDialog();

      /**
       * Returns control password specified by user (in plain text).
       */
      ::string getPassword() const;
   protected:
      virtual bool onInitDialog();
      virtual bool onNotify(::u32 controlID, ::lparam data);
      virtual bool onCommand(::u32 controlID, ::u32 notificationID);
      virtual bool onDestroy();
   private:
      ::string m_password;
      ::innate_subsystem::TextBox m_passwordTextBox;
   };
}//   namespace remoting_control_desktop
