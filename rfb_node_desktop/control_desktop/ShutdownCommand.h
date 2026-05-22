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

#pragma once


#include "subsystem/platform/Command.h"

#include "ControlProxy.h"


namespace remoting_control_desktop
{
   /**
    * Command that invokes remote shutdown() method of Remoting Node
    * using control transport.
    */
   class ShutdownCommand : public ::subsystem::Command
   {
   public:
      /**
       * Creates command.
       * @param serverControl proxy.
       */
      ShutdownCommand(ControlProxy * pcontrolproxy);
      /**
       * Destroys command.
       */
      virtual ~ShutdownCommand();

      /**
       * Executes command.
       *
       * Inhrited from Command abstract class.
       *
       * @throws ::io_exception on io error, RemoteException on server side error.
       */
      void onRunCommand() override;
   private:
      /**
       * Proxy to some of Remoting Node control methods.
       */
      ::pointer < ControlProxy > m_pcontrolproxy;
   };
} // namespace remoting_control_desktop

