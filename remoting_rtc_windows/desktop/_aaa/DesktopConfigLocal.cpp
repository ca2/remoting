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
#include "DesktopConfigLocal.h"
#include "remoting/remoting_rtc/node_config/Configurator.h"
#include "windows/WindowsInputBlocker.h"

namespace remoting_rtc_windows
{

   DesktopConfigLocal::DesktopConfigLocal():
   m_plogwriter(nullptr)
   {

   }

   DesktopConfigLocal::~DesktopConfigLocal()
   {
      //delete m_pinputblocker;
   }


   void DesktopConfigLocal::initialize_desktop_config_local(Configurator *pconfigurator, subsystem::LogWriter *plogwriter)
   {
      m_plogwriter = plogwriter;
      m_pconfigurator = pconfigurator;
      m_pinputblocker = create_newø<WindowsInputBlocker >();
      m_pinputblocker->initialize_input_blocker(plogwriter);

   }

   void DesktopConfigLocal::updateByNewSettings()
   {
      ::remoting_rtc_node::ServerConfig *pserverconfig = m_pconfigurator->getServerConfig();
      bool hardBlocking = pserverconfig->isBlockingLocalInput();
      m_pinputblocker->setKeyboardBlocking(hardBlocking);
      m_pinputblocker->setMouseBlocking(hardBlocking);

      bool softBlocking = pserverconfig->isLocalInputPriorityEnabled();
      unsigned int interval = pserverconfig->getLocalInputPriorityTimeout() * 1000;
      m_pinputblocker->setSoftKeyboardBlocking(softBlocking, interval);
      m_pinputblocker->setSoftMouseBlocking(softBlocking, interval);
   }

   bool DesktopConfigLocal::isRemoteInputAllowed() { return m_pinputblocker->isRemoteInputAllowed(); }

   class ::time DesktopConfigLocal::getLastInputTime() const { return m_pinputblocker->getLastInputTime(); }

   void DesktopConfigLocal::correctLastTime(const class ::time & time) { m_pinputblocker->correctLastTime(time); }


} // namespace remoting_rtc_windows
 