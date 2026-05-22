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


#include "acme/input_output/Channel.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "acme/parallelization/happening.h"
//#include "log_writer/LogWriter.h"


namespace remoting_rfb
{


   class CLASS_DECL_REMOTING_RFB  ReconnectingChannel : public Channel
   {
   public:

      bool m_bIsClosed;

      ::pointer < Channel > m_pchannel;
      ::pointer < Channel > m_pchannelOld;
      bool m_bChannelWasChanged;
      lockable_critical_section m_criticalsectionChannel;

      ::happening m_happeningTimer;
      class CLASS_DECL_REMOTING_RFB  ::time m_timeTimeout;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;



      //ReconnectingChannel(::u32 timeOut, ::subsystem::LogWriter * plogwriter);
      ReconnectingChannel();
      ReconnectingChannel(const class CLASS_DECL_REMOTING_RFB  ::time & timeTimeout, ::subsystem::LogWriter * plogwriter);
      ~ReconnectingChannel() override;


      virtual void initialize_reconnecting_channel(const class CLASS_DECL_REMOTING_RFB  ::time & timeTimeout, ::subsystem::LogWriter * plogwriter);

      memsize read(void *buffer, memsize len) override;
      memsize defer_write(const void *buffer, memsize len) override;

      // Replaces invalid channel by the new valid.
      // At the next the read()/write() function call read()/write()
      // generates the ReconnectException if this is not the first initialization
      void replaceChannel(Channel *newChannel);

      // Closes connection and break all blocked operation.
      // @throw ::subsystem::Exception on error.
      virtual void close() override;

      virtual memsize available() override { return 0; };

   //private:
      // @param funName - is a function name that will be placed to the
      // ReconnectException text.
      // @throw ReconnectException on reconnect detection.
      // @throw ::io_exception on other errors.
      Channel *getChannel(const ::scoped_string &scopedstrFunName);

      // @param funName - is a function name that will be placed to the
      // ReconnectException text.
      // @param channel - currently using transport.
      // @throw ReconnectException on reconnect detection.
      // @throw ::io_exception on other errors.
      void waitForReconnect(const ::scoped_string &scopedstrFunName, Channel *channel);

   };


}  // namespace remoting_rfb




