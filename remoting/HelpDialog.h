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


#include "subsystem_apex/Dialog.h"
#include "subsystem_apex/TextBox.h"
#include "resource.h"

namespace remoting_remoting
{
    class HelpDialog : public ::subsystem_apex::Dialog
    {
    public:
        HelpDialog();

    //protected:
        bool onInitDialog() ;
        bool onCommand(unsigned int controlID, unsigned int notificationID);
        ::pointer < ::subsystem_apex::TextBox >  m_shelp;

    };
}//namespace remoting_remoting
