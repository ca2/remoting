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


#include "subsystem/platform/Command.h"

#include "remoting/remoting_rfb/node/Notificator.h"

namespace remoting_rfb_node
{
   /**
    * Command that executes remote method using ControlProxy and catchs all of
    * exception that can be thown by executing of such command.
    */
   class CLASS_DECL_REMOTING_RFB ControlCommand : public ::subsystem::Command
   {
   public:
      /**
       * Creates control command.
       * @param command real command.
       * @param notificator [optional] notificator to report about error.
       */
      ControlCommand(::subsystem::Command *command, Notificator *notificator = 0);
      /**
       * Destroys command.
       */
      virtual ~ControlCommand();

      /**
       * Executes command.
       */
      void onRunCommand() override;

      /**
       * Checks result of last onThreadMain() method call.
       * @note if called before onThreadMain() then it will return false.
       * @return true if onThreadMain() doesn't real command (passed to constructor) don't raise any exceptions,
       * false otherwise.
       */
      bool executionResultOk() const;

   protected:
      /**
       * Real command.
       */
      ::pointer < Command > m_pcommand;
      /**
       * Error notificator.
       */
      ::pointer < Notificator > m_pnotificator;
      /**
       * Flag determinates successfull execution of command.
       */
      bool m_successfull;
   };
} // namespace remoting_control_desktop

