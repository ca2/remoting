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
#include "framework.h"
#include "DesktopSrvDispatcher.h"
#include "ReconnectException.h"
#include "subsystem/_common_header.h"
#include "subsystem/platform/Exception.h"

namespace remoting_rfb
{


   // DesktopSrvDispatcher::DesktopSrvDispatcher(BlockingGate *pblockinggate, AnEventListener *extErrorListener,
   //                                            ::subsystem::LogWriter * plogwriter) :
   //     m_pcontrolgate(pcontrolgate), m_extErrorListener(extErrorListener), m_plogwriter = plogwriter;
   // {
   // }

   DesktopSrvDispatcher::DesktopSrvDispatcher():
   m_pcontrolgate(nullptr),m_plogwriter(nullptr)
   {


   }


   DesktopSrvDispatcher::DesktopSrvDispatcher(BlockingGate *pblockinggate,
 //AnEventListener *m_extTerminationListener,
                   const ::procedure &procedureDesktopSrvDispatcher, ::subsystem::LogWriter * plogwriter) :
   DesktopSrvDispatcher()
   {
      initialize_desktop_srv_dispatcher(pblockinggate, procedureDesktopSrvDispatcher, plogwriter);
   }



   DesktopSrvDispatcher::~DesktopSrvDispatcher()
   {
  //    setThreadToFinish();
//      waitThreadToFinish();
   }

void DesktopSrvDispatcher::destroy()
{
   
   ::subsystem::Thread::destroy();
   // terminateThread();
   // waitThreadToFinish();
}


void DesktopSrvDispatcher::initialize_desktop_srv_dispatcher(BlockingGate *pblockinggate,
                     // AnEventListener *m_extTerminationListener,
                     const ::procedure &procedureDesktopSrvDispatcher, ::subsystem::LogWriter * plogwriter)
   {

      m_pcontrolgate = pblockinggate;
      m_procedureDesktopSrvDispatcher = procedureDesktopSrvDispatcher;
      m_plogwriter = plogwriter;

   }


   void DesktopSrvDispatcher::onTermThread() {}

   void DesktopSrvDispatcher::notifyOnError()
   {
      //if (m_extErrorListener)
      //{
        // m_extErrorListener->onAnObjectEvent();
      //}
      if (m_procedureDesktopSrvDispatcher)
      {

         m_procedureDesktopSrvDispatcher();

      }
   }

   void DesktopSrvDispatcher::onThreadMain()
   {
      while (!isThreadTerminating())
      {
         try
         {
            m_plogwriter->debug("DesktopSrvDispatcher reading code");
            ::u8 code = m_pcontrolgate->readUInt8();
            m_plogwriter->debug("DesktopSrvDispatcher, code {} recieved", code);
            ::map<::u8, ClientListener *>::iterator iter = m_handlers.find(code);
            if (iter == m_handlers.end())
            {
               ::string errMess;
               errMess.format("Unhandled {} code has been "
                               "received from a client",
                               (::i32)code);
               throw ::subsystem::Exception(errMess);
            }
            (*iter).m_element2->onRequest(code, m_pcontrolgate);
         }
         catch (ReconnectException &)
         {
            m_plogwriter->debug("The DesktopServerApplication dispatcher has been reconnected");
         }
         catch (::exception &e)
         {
            m_plogwriter->error("The DesktopServerApplication dispatcher has "
                                "failed with error: {}",
                                e.get_message());
            notifyOnError();
            //terminateThread();
            setThreadToFinish();
         }
         Thread::threadYield();
      }
      m_plogwriter->debug("The DesktopServerApplication dispatcher has been stopped");
   }

   void DesktopSrvDispatcher::registerNewHandle(::u8 code, ClientListener *listener)
   {
      m_handlers[code] = listener;
   }


} // namespace remoting_rfb
