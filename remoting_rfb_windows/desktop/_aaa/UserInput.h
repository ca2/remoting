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


#include "subsystem/_common_header.h"
//#include "remoting/remoting_rfb/region/::i32_point.h"
#include "remoting/remoting_rfb/desktop_ipc/BlockingGate.h"

#include "remoting/remoting_rfb/region/Region.h"


namespace remoting_windows
{

   // This class  will be an abstract interface for user input such as keyboard,
   // mouse pointer, e.t.c., on the server side.
   class CLASS_DECL_REMOTING_RFB UserInput :
   virtual public ::particle
   {
   public:
      UserInput();
      virtual ~UserInput();

      // FIXME: It's no good idea to place this function to here.
      // Because it uses only for the UserInputClient class.
      virtual void sendInit(BlockingGate *pblockinggate) {}

      // Client to server user inputs
      virtual void setNewClipboard(const ::scoped_string &newClipboard) = 0;
      // By the keyFlag argument will be set the mouse button state as described in
      // the rfb protocol.
      virtual void setMouseEvent(const ::i32_point pointNewPosition, unsigned char keyFlag) = 0;
      virtual void setKeyboardEvent(unsigned int keySym, bool down) = 0;
      virtual void getCurrentUserInfo(::string &desktopName, ::string &userName) = 0;

      virtual void getPrimaryDisplayCoords(::i32_rectangle & rectangle) = 0;
      virtual void getDisplayNumberCoords(::i32_rectangle & rectangle, unsigned char dispNumber) = 0;
      virtual ::int_rectangle_array_base getDisplaysCoords() = 0;
      virtual void getNormalizedRect(::i32_rectangle & rectangle) = 0;

      virtual void getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle) = 0;
      virtual ::operating_system::window getWindowHandleByName(const ::scoped_string &windowName) = 0;

      virtual void getApplicationRegion(unsigned int procId, ::remoting_rfb::Region & region) = 0;
      virtual bool isApplicationInFocus(unsigned int procId) = 0;


   };


} // namespace remoting_windows






