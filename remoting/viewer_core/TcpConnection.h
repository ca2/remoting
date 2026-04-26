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


//#include "log_writer/LogWriter.h"
#include "remoting/remoting/network/RfbInputGate.h"
#include "remoting/remoting/network/RfbOutputGate.h"
//#include "remoting/remoting/network/socket/SocketIPv4.h"
//#include "remoting/remoting/network/socket/SocketStream.h"
//#include "subsystem/thread/critical_section.h"
#include "acme/input_output/BufferedInputStream.h"


namespace remoting
{
   class CLASS_DECL_REMOTING TcpConnection :

   virtual public ::particle
   {
   public:
      //TcpConnection(::subsystem::LogWriter * plogwriter);
      TcpConnection();
      ~TcpConnection() override;


      virtual void initialize_tcp_connection(::subsystem::LogWriter * plogwriter);

      void bind(const ::scoped_string & scopedstrHost, unsigned short port);
      void bind(::subsystem::SocketIPv4Interface *socket);
      void bind(RfbInputGate *input, RfbOutputGate *output);

      void connect();
      void close();

      RfbInputGate *getInput() const;
      RfbOutputGate *getOutput() const;
   ;;private:
      ::string m_host;
      unsigned short m_port;
      ::pointer < ::subsystem::SocketIPv4Interface > m_psocket;
      bool m_socketOwner;
      ::pointer < ::subsystem::SocketStream > m_psocketstream;
      ::pointer < ::BufferedInputStream > m_pbufInput;
      ::pointer < RfbInputGate > m_pinput;
      ::pointer < RfbOutputGate > m_poutput;
      bool m_RfbGatesOwner;

      bool m_wasBound;
      bool m_wasConnected;
      bool m_isEstablished;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      mutable critical_section m_connectLock;
   };

} // namespace remoting


