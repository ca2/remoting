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


#include "apex/innate_subsystem/Dialog.h"
#include "apex/innate_subsystem/Control.h"
#include "apex/innate_subsystem/TextBox.h"

namespace remoting_remoting
{


   class NewFolderDialog : public ::innate_subsystem::Dialog
   {
   public:
      NewFolderDialog();
      ~NewFolderDialog();


      virtual void initialize_new_folder_dialog(::innate_subsystem::Control *parent);

      void setFileName(const ::scoped_string & scopedstrFilename);
      ::string getFileName();

      //protected:

      //
      // Inherited from BaseDialog
      //

      virtual bool onInitDialog();
      virtual bool onNotify(unsigned int controlID, ::lparam data);
      virtual bool onCommand(unsigned int controlID, unsigned int notificationID);
      virtual bool onDestroy();

      //
      // Button event handlers
      //

      virtual void onOkButtonClick();
      virtual void onCancelButtonClick();

      //private:

      virtual void initControls();

      //protected:

      //
      // Controls
      //

      ::pointer < ::innate_subsystem::Control > m_pcontrolLabel;
      ::pointer < ::innate_subsystem::TextBox > m_ptextboxFileName;

      ::string m_strFileName;
   };


} // namespace remoting_remoting



