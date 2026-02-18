// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "SecurityPipeServer.h"
#include "win_system/PipeServer.h"
#include "win_system/PipeClient.h"
#include "io_lib/DataInputStream.h"
#include "io_lib/DataOutputStream.h"
//#include "win_system/Environment.h"
#include "win_system/WinHandles.h"
#include "ConnectionTimer.h"
#include <time.h>

SecurityPipeServer::SecurityPipeServer(Channel *tempPublChan, unsigned int bufferSize)
: m_secChannel(0),
  m_bufferSize(bufferSize)
{
  try {
    generateSecConnection(tempPublChan);
    makeSure();
  } catch (...) {
    if (m_secChannel != 0) delete m_secChannel;
    throw;
  }
}

SecurityPipeServer::~SecurityPipeServer()
{
}

Channel *SecurityPipeServer::getChannel()
{
  Channel *chan = m_secChannel;
  m_secChannel = 0;
  return chan;
}

void SecurityPipeServer::generateSecConnection(Channel *tempPublChan)
{
  NamedPipe *otherSideChannel = 0;

  try {
    DataInputStream svcInput(tempPublChan);
    DataOutputStream svcOutput(tempPublChan);

    unsigned int procId = svcInput.readUInt32();
    // Check the id. If it is "right" process then generate
    // transport handles specially for it.
    if (!Environment::isItTheSamePathAsCurrent(procId)) {
      throw ::remoting::Exception("The process that has requested connection"
                      " to the log server has not access right");
    }

    // Give to process exclusive pipe handles
    ::string randomName;
    srand((unsigned)time(0));
    for (int i = 0; i < 20; i++) {
      randomName.appendChar('a' + rand() % ('z' - 'a'));
    }
    PipeServer pipeServer(randomName, m_bufferSize, 0, 1000);
    otherSideChannel = PipeClient::connect(randomName, m_bufferSize);
    m_secChannel = pipeServer.accept();

    HANDLE otherSideHandle = otherSideChannel->getHandle();
    svcOutput.writeUInt64((unsigned long long)WinHandles::assignHandleFor(otherSideHandle, procId, false, false));
  } catch (...) {
    if (otherSideChannel != 0) delete otherSideChannel;
    throw;
  }
  if (otherSideChannel != 0) delete otherSideChannel;
}

void SecurityPipeServer::onTimeTimer()
{
  m_secChannel->close();
}

void SecurityPipeServer::makeSure()
{
  DataInputStream inputGate(m_secChannel);
  DataOutputStream outputGate(m_secChannel);
  // Acknowledgement of the receipt
  unsigned int timeOut = 10000; // milliseconds
  ConnectionTimer connTimer(this, timeOut);
  // If no byte received during time out interval connTimer has break
  // read operation and the make sure throws an ::remoting::Exception.
  inputGate.readInt8();
}
