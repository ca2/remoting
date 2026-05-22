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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once

//#include "util/Exception.h"
//#include "util/Singleton.h"
//#include "log-writer/LogWriter.h"

//#include "subsystem_windows/node/Impersonator.h"

#include "desktop/Desktop.h"

namespace remoting
{


   namespace file_transfer
   {
      class CLASS_DECL_REMOTING FileTransferSecurity :
       //virtual public ::subsystem_windows::Impersonator
   virtual public ::particle
      {
      public:


         bool m_hasAccess;

         ::remoting_rtc::Desktop *m_pdesktop;

         ::pointer < ::subsystem::LogWriter > m_plogwriter;
         ::pointer < ::remoting_rtc_node::Configurator > m_pconfigurator;


         //FileTransferSecurity(::remoting_rtc_node::Configurator * pconfigurator, ::remoting_rtc::Desktop *desktop, ::subsystem::LogWriter *log);
         
         FileTransferSecurity();
         ~FileTransferSecurity() override;

         virtual void initialize_file_transfer_security(::remoting_rtc_node::Configurator * pconfigurator, ::remoting_rtc::Desktop *desktop, ::subsystem::LogWriter *log);

         // Sets access rights for calling process for execution
         // file transfer request handler code.
         virtual void beginMessageProcessing();
         // Checks result and throws exception if caller
         // must not onThreadMain file transfer code and return error
         // to client.
         virtual void throwIfAccessDenied(); //throw(Exception);
         // Sets previous (before startMessageProcessing call) access rights
         // for calling process.
         virtual void endMessageProcessing();

      protected:
         // bool m_hasAccess;
         //
         // ::remoting_rtc::Desktop *m_pdesktop;
         //
         // ::pointer < ::subsystem::LogWriter > m_plogwriter;
      };


   } // namespace file_transfer


} // namespace remoting



