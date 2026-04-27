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
#include "framework.h"
#include "ReconnectingChannel.h"
//#include "subsystem/platform/class ::time.h"
#include "remoting/remoting/desktop_ipc/ReconnectException.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "subsystem/platform/Exception.h"

namespace remoting
{


   ReconnectingChannel::ReconnectingChannel() :
       m_iTimeout(0), m_bIsClosed(false), m_pchannel(0), m_pchannelOld(0), m_bChannelWasChanged(false), m_plogwriter(nullptr)
   {
   }

   ReconnectingChannel::~ReconnectingChannel()
   {
      if (m_pchannel != 0)
      {
         delete m_pchannel;
      }
      if (m_pchannelOld != 0)
      {
         delete m_pchannelOld;
      }
   }


   void ReconnectingChannel::initialize_reconnecting_channel(unsigned int timeOut, ::subsystem::LogWriter * plogwriter)
   {

      m_iTimeout = timeOut;
      m_plogwriter = plogwriter;
   }

   void ReconnectingChannel::close()
   {
      critical_section_lock al(&m_criticalsectionChannel);
      m_bIsClosed = true;
      if (m_pchannel != 0)
      {
         m_pchannel->close();
      }
      if (m_pchannelOld != 0)
      {
         m_pchannelOld->close();
      }
      m_happeningTimer.set_happening();
   }

   void ReconnectingChannel::replaceChannel(Channel *newChannel)
   {
      critical_section_lock al(&m_criticalsectionChannel);
      if (m_pchannel != 0)
      {
         m_bChannelWasChanged = true; // Toggle to true except first initialization
      }
      if (m_pchannelOld != 0)
      {
         delete m_pchannel; // This channel in this case is guaranted doesn't use.
      }
      m_pchannelOld = m_pchannel;
      m_pchannel = newChannel; // Now we are the owner.
      m_happeningTimer.set_happening();
   }

   Channel *ReconnectingChannel::getChannel(const ::scoped_string &scopedstrFunName)
   {
      if (m_bIsClosed)
      {
         ::string errMess;
         errMess.format("The {}() function has failed:"
                         " connection has already been closed.",
                         scopedstrFunName);
         throw ::io_exception(error_io, errMess);
      }
      Channel *channel;
      {
         critical_section_lock al(&m_criticalsectionChannel);
         // Clean m_pchannelOld
         if (m_pchannelOld != 0)
         {
            delete m_pchannelOld;
            m_pchannelOld = 0;
         }
         if (m_bChannelWasChanged)
         {
            m_bChannelWasChanged = false;
            ::string errMess;
            errMess.format("Transport was reconnected outside from"
                            " the {}() function. The {}()"
                            " function at this time will be aborted.",
                            scopedstrFunName, scopedstrFunName);
            throw ReconnectException(errMess);
         }
         channel = m_pchannel;
      }
      return channel;
   }


   memsize ReconnectingChannel::defer_write(const void *buffer, memsize len)
   {
      Channel *channel = getChannel("write");

      try
      {
         if (channel == 0)
         {
            throw ::subsystem::Exception("write() function stopped because transport"
                                         " has not been initialized yet.");
         }
         return channel->defer_write(buffer, len);
      }
      catch (::exception &e)
      {
         m_plogwriter->error(e.get_message());
         waitForReconnect("write", channel);
      }

      return 0; // Call by an out caller again!
   }

   memsize ReconnectingChannel::read(void *buffer, memsize len)
   {
      Channel *channel = getChannel("read");

      try
      {
         if (channel == 0)
         {
            throw ::subsystem::Exception("read() function stopped because transport"
                                         " has not been initialized yet.");
         }
         return channel->read(buffer, len);
      }
      catch (::exception &e)
      {
         m_plogwriter->error(e.get_message());
         waitForReconnect("read", channel);
      }

      return 0; // Call by an out caller again!
   }

   void ReconnectingChannel::waitForReconnect(const ::scoped_string &scopedstrFunName, Channel *channel)
   {
      // Wait until transport has been initialized or time out elapsed.
      auto startTime = ::time::now();
      bool success = false;
      while (!success)
      {
         unsigned int timeForWait = maximum((m_iTimeout - startTime.elapsed()).integral_millisecond(), 0);
         if (timeForWait == 0 || m_bIsClosed)
         { // Break this function with
           // critical error
            ::string errMess;
            errMess.format("The ReconnectingChannel::{}() function"
                            " failed.",
                            scopedstrFunName);
            throw ::io_exception(error_io, errMess);
         }
         m_happeningTimer.wait(timeForWait * 1_ms);
         critical_section_lock al(&m_criticalsectionChannel);
         if (m_pchannel != channel)
         {
            m_bChannelWasChanged = false; // Reconnection catched in this place!
                                      // (other must don't know about this)
            success = true;
         }
      }
      m_plogwriter->information("ReconnectingChannel was successfully reconnected.");
      if (channel != 0)
      { // If this is not the first initialization
         ::string errMess;
         errMess.formatf("Transport was reconnected in the"
                         " {}() function. The {}()"
                         " function() at this time will be aborted",
                         scopedstrFunName, scopedstrFunName);
         throw ReconnectException(errMess);
      }
   }


} // namespace remoting
