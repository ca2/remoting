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
    * Command that copies current configuration of remote Remoting Node
    * to local application configuration.
    */
   class UpdateLocalConfigCommand : public ::subsystem::Command
   {
   public:
      /**
       * Creates command.
       * @param proxy ready to use control proxy.
       */
      UpdateLocalConfigCommand(::remoting_rfb_node::Configurator * pconfigurator, ControlProxy *proxy);
      /**
       * Destructor.
       */
      virtual ~UpdateLocalConfigCommand();
      /**
       * Executes command.
       * @throws ::io_exception, RemoteException.
       */
      void onRunCommand() override;
   private:
      ::pointer < ControlProxy > m_pcontrolproxy;
      ::pointer < ::remoting_rfb_node::Configurator > m_pconfigurator;
   };
} //   namespace remoting_control_desktop

