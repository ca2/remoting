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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "ReconnectingChannel.h"
//#include "util/::earth::time.h"
#include "desktop_ipc/ReconnectException.h"
#include "thread/AutoLock.h"

ReconnectingChannel::ReconnectingChannel(unsigned int timeOut, LogWriter *log)
: m_timeOut(timeOut),
  m_isClosed(false),
  m_channel(0),
  m_oldChannel(0),
  m_chanWasChanged(false),
  m_log(log)
{
}

ReconnectingChannel::~ReconnectingChannel()
{
  if (m_channel != 0) {
    delete m_channel;
  }
  if (m_oldChannel != 0) {
    delete m_oldChannel;
  }
}

void ReconnectingChannel::close()
{
  AutoLock al(&m_chanMut);
  m_isClosed = true;
  if (m_channel != 0) {
    m_channel->close();
  }
  if (m_oldChannel != 0) {
    m_oldChannel->close();
  }
  m_timer.notify();
}

void ReconnectingChannel::replaceChannel(Channel *newChannel)
{
  AutoLock al(&m_chanMut);
  if (m_channel != 0) {
    m_chanWasChanged = true; // Toggle to true except first initialization
  }
  if (m_oldChannel != 0) {
    delete m_channel; // This channel in this case is guaranted doesn't use.
  }
  m_oldChannel = m_channel;
  m_channel = newChannel; // Now we are the owner.
  m_timer.notify();
}

Channel *ReconnectingChannel::getChannel(const ::scoped_string & scopedstrFunName)
{
  if (m_isClosed) {
    ::string errMess;
    errMess.formatf("The {}() function has failed:"
                   " connection has already been closed.",
                   funName);
    throw ::io_exception(errMess);
  }
  Channel *channel;
  {
    AutoLock al(&m_chanMut);
    // Clean m_oldChannel
    if (m_oldChannel != 0) {
      delete m_oldChannel;
      m_oldChannel = 0;
    }
    if (m_chanWasChanged) {
      m_chanWasChanged = false;
      ::string errMess;
      errMess.formatf("Transport was reconnected outside from"
                     " the {}() function. The {}()"
                     " function at this time will be aborted.",
                     funName,
                     funName);
      throw ReconnectException(errMess);
    }
    channel = m_channel;
  }
  return channel;
}

size_t ReconnectingChannel::write(const void *buffer, size_t len)
{
  Channel *channel = getChannel("write");

  try {
    if (channel == 0) {
      throw ::remoting::Exception("write() function stopped because transport"
                      " has not been initialized yet.");
    }
    return channel->write(buffer, len);
  } catch (::exception &e) {
    m_log->error(e.get_message());
    waitForReconnect("write", channel);
  }

  return 0; // Call by an out caller again!
}

size_t ReconnectingChannel::read(void *buffer, size_t len)
{
  Channel *channel = getChannel("read");

  try {
    if (channel == 0) {
      throw ::remoting::Exception("read() function stopped because transport"
                      " has not been initialized yet.");
    }
    return channel->read(buffer, len);
  } catch (::exception &e) {
    m_log->error(e.get_message());
    waitForReconnect("read", channel);
  }

  return 0; // Call by an out caller again!
}

void ReconnectingChannel::waitForReconnect(const ::scoped_string & scopedstrFunName,
                                         Channel *channel)
{
  // Wait until transport has been initialized or time out elapsed.
  ::earth::time startTime = ::earth::time::now();
  bool success = false;
  while (!success) {
    unsigned int timeForWait = max((int)m_timeOut - 
                                   (int)(::earth::time::now() -
                                         startTime).getTime(),
                                   0);
    if (timeForWait == 0 || m_isClosed) { // Break this function with
                                          // critical error
      ::string errMess;
      errMess.formatf("The ReconnectingChannel::{}() function"
                     " failed.", funName);
      throw ::io_exception(errMess);
    }
    m_timer.waitForEvent(timeForWait);
    AutoLock al(&m_chanMut);
    if (m_channel != channel) {
      m_chanWasChanged = false; // Reconnection catched in this place!
                                // (other must don't know about this)
      success = true;
    }
  }
  m_log->information("ReconnectingChannel was successfully reconnected.");
  if (channel != 0) { // If this is not the first initialization
    ::string errMess;
    errMess.formatf("Transport was reconnected in the"
                   " {}() function. The {}()"
                   " function() at this time will be aborted",
                   funName, funName);
    throw ReconnectException(errMess);
  }
}
