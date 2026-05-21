// Copyright (C) 2012 GlavSoft LLC.
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
#include "subsystem/socket/SocketAddressIPv4.h"
#include "subsystem/socket/SocketIPv4.h"
#include "subsystem/socket/SocketStream.h"
#include "TcpConnection.h"

//#include "subsystem_bsd_sockets/platform/subsystem.h"


namespace remoting_client
{
   // TcpConnection::TcpConnection(::subsystem::LogWriter * plogwriter)
   // : m_plogwriter(plogwriter),
   // m_socketOwner(false),
   // //m_bufInput(0),
   // m_RfbGatesOwner(false)
   // {
   //    m_port = 0;
   //    //m_socket = 0;
   //    //m_socketStream = 0;
   //    //m_input = 0;
   //    //m_output = 0;
   //    m_wasBound = false;
   //    m_wasConnected = false;
   //    m_isEstablished = false;
   // }

   TcpConnection::TcpConnection()
: //m_plogwriter(plogwriter),
m_socketOwner(false),
//m_bufInput(0),
m_RfbGatesOwner(false)
   {
      m_port = 0;
      //m_socket = 0;
      //m_socketStream = 0;
      //m_input = 0;
      //m_output = 0;
      m_wasBound = false;
      m_wasConnected = false;
      m_isEstablished = false;
   }
   TcpConnection::~TcpConnection()
   {
      // if socket is defined, then need delete gates and socket stream
      if (m_psocket) {
         if (m_pinput && m_RfbGatesOwner) {
            try {
               //delete m_input;
               m_pinput.release();
            } catch (...) {
            }
         }

         if (m_poutput && m_RfbGatesOwner) {
            try {
               //delete m_output;
               m_poutput.release();
            } catch (...) {
            }
         }

         if (m_psocketstream) {
            try {
               //delete m_socketStream;
               m_psocketstream.release();
            }
            catch (...) {
            }
         }
         if (m_pbufInput) {
            try {
               //delete m_bufInput;
               m_pbufInput.release();
            }
            catch (...) {
            }
         }

      }

      // if host and port is defined, then need delete socket
      try {
         if (m_psocket && !m_host.is_empty() && m_port && m_socketOwner) {
            //delete m_socket;
            m_psocket.release();
            //m_socket = NULL;
         }
      } catch (...) {
      }
   }


   void TcpConnection::initialize_tcp_connection(::subsystem::LogWriter * plogwriter)
   {
      m_socketOwner = false;
      //m_bufInput(0),
      m_RfbGatesOwner = false;

      m_plogwriter = plogwriter;
      m_port = 0;
      //m_socket = 0;
      //m_socketStream = 0;
      //m_input = 0;
      //m_output = 0;
      m_wasBound = false;
      m_wasConnected = false;
      m_isEstablished = false;
   }

   void TcpConnection::bind(const ::scoped_string & scopedstrHost, unsigned short port)
   {
      critical_section_lock al(&m_connectLock);
      if (m_wasBound) {
         throw ::subsystem::Exception("Tcp-connection already bound");
      }
      m_host = scopedstrHost;
      m_port = port;
      m_psocket.release();
      m_pinput.release();
      m_poutput.release();
      m_wasBound = true;
   }

   void TcpConnection::bind(subsystem::SocketIPv4Interface * psocket)
   {
      critical_section_lock al(&m_connectLock);
      if (m_wasBound) {
         throw ::subsystem::Exception("Tcp-connection already bound");
      }
      m_host = "";
      m_port = 0;
      m_psocket = psocket;
      m_pinput.release();
      m_poutput.release();
      m_wasBound = true;
   }

   void TcpConnection::bind(::remoting::RfbInputGate * pinput, ::remoting::RfbOutputGate * poutput)
   {
      critical_section_lock al(&m_connectLock);
      if (m_wasBound) {
         throw ::subsystem::Exception("Tcp-connection already bound");
      }
      m_host = "";
      m_port = 0;
      m_psocket.release();
      m_pinput = pinput;
      m_poutput = poutput;
      m_wasBound = true;
   }

   void TcpConnection::connect()
   {
      // if connection is already established, then method do nothing.
      {
         critical_section_lock al(&m_connectLock);
         m_wasConnected = true;
      }
      // need create to gates
      if (!m_pinput && !m_poutput) {
         // need create to socket
         if (!m_psocket) {
            if (!m_host.is_empty() && m_port != 0) {
               ::subsystem::SocketAddressIPv4 ipAddress;

               ipAddress.initialize_socket_address_ipv4(m_host, m_port);

               ::string ipAddressString;
               ipAddressString = ipAddress.toString();
               m_plogwriter->debug("Connecting to the host \"{}:{}\" ({}:{})...",
                                   m_host, m_port,
                                   ipAddressString, m_port);

               m_psocket = ::system()->createø< ::subsystem::SocketIPv4Interface >();
               //m_psocket->initialize_socket_ip4();
               m_socketOwner = true;
               m_psocket->connect(ipAddress);
               m_psocket->enableNaggleAlgorithm(false);
            } else {
               throw ::subsystem::Exception("Connection parameters (host, port, socket, gates) is empty.");
            }
         }

         m_plogwriter->debug("Initialization of socket stream and input/output gates...");
         emplace_newø(m_psocketstream, m_psocket);
         //m_psocketstream->initialize_socket_stream(m_psocket);
         emplace_newø(m_pbufInput, m_psocketstream);
         //m_pbufInput->initialize_buffered_input_stream(m_psocketstream);
         emplace_newø(m_pinput, m_pbufInput);
         //m_pinput->initialize_rfb_input_gate(m_pbufInput);
         emplace_newø(m_poutput, m_psocketstream);
         //m_poutput->initialize_rfb_output_gate(m_psocketstream);
         m_RfbGatesOwner = true;
      } else {
         ASSERT(m_pinput && m_poutput);
      }
      {
         critical_section_lock al(&m_connectLock);
         m_isEstablished = true;
      }
   }

   void TcpConnection::close()
   {
      if (!m_host.is_empty() && m_port != 0) {
         if (m_psocketstream) {
            m_psocketstream->close();
         }
      }
   }

   ::remoting::RfbInputGate *TcpConnection::getInput() const
   {
      {
         critical_section_lock al(&m_connectLock);
         if (!m_isEstablished) {
            throw ::subsystem::Exception("Connecting has not been established");
         }
      }
      return m_pinput;
   }

   ::remoting::RfbOutputGate *TcpConnection::getOutput() const
   {
      {
         critical_section_lock al(&m_connectLock);
         if (!m_isEstablished) {
            throw ::subsystem::Exception("Connection has not been established");
         }
      }
      return m_poutput;
   }


} // namespace remoting_client
