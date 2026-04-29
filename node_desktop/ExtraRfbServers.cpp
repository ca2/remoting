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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "ExtraRfbServers.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "remoting/remoting/node_config/ServerConfig.h"

namespace remoting_node_desktop
{


   ExtraRfbServers::Conf::Conf() : acceptConnections(false), loopbackOnly(false), extraPorts() {}

   ExtraRfbServers::Conf::Conf(const Conf &other) :
       acceptConnections(other.acceptConnections), loopbackOnly(other.loopbackOnly), extraPorts(other.extraPorts)
   {
   }

   ExtraRfbServers::Conf &ExtraRfbServers::Conf::operator=(const Conf &other)
   {
      acceptConnections = other.acceptConnections;
      loopbackOnly = other.loopbackOnly;
      extraPorts = other.extraPorts;
      return *this;
   }

   bool ExtraRfbServers::Conf::equals(const Conf *other)
   {
      return (acceptConnections == other->acceptConnections && loopbackOnly == other->loopbackOnly &&
              extraPorts.equals(&other->extraPorts));
   }
   //(ExtraRfbServers::ExtraRfbServers()::subsystem::LogWriter * log) :
   //    m_servers(), m_effectiveConf(), m_plogwriter = plogwriter;
   //{
   //}

   ExtraRfbServers::ExtraRfbServers()
   {



   }

   ExtraRfbServers::~ExtraRfbServers()
   {
      try
      {
         shutDown();
      }
      catch (...)
      {
      }
   }


      void ExtraRfbServers::initialize_extra_rfb_servers(::remoting_node::Configurator * pconfigurator, ::subsystem::LogWriter *plogwriter)
   {
         m_pconfigurator = pconfigurator;
      m_plogwriter = plogwriter;
         //::subsystem::LogWriter * plogwriter) : m_servers(), m_effectiveConf(), m_plogwriter = plogwriter;
   }


   bool ExtraRfbServers::reload(bool asService, RfbClientManager *mgr)
   {
      m_plogwriter->debug("Considering to reload extra RFB servers");

      Conf newConf;
      getConfiguration(&newConf);
      bool noConfigChanges = newConf.equals(&m_effectiveConf);
      bool enoughServers = (newConf.extraPorts.count() == m_servers.size());
      m_plogwriter->debug("Same Extra Ports configuration = {}, enough servers = {}", (int)noConfigChanges,
                          (int)enoughServers);

      if (noConfigChanges && enoughServers)
      {
         return true; // no work needed, no errors encountered
      }

      // Either configuration was actually changed, or our number of actually
      // running servers does not match the configuration. In either case,
      // restart the servers.
      m_plogwriter->debug("Need to reconfigure extra RFB servers");
      shutDown();
      return startUp(asService, mgr);
   }

   void ExtraRfbServers::shutDown()
   {
      m_plogwriter->debug("Requested to shut down extra RFB servers");

      for (auto i = m_servers.begin(); i != m_servers.end(); i++)
      {
         int port = (*i)->getBindPort();
         m_plogwriter->debug("Stopping extra RFB server at port {}", port);
         delete *i;
         m_plogwriter->debug("Stopped extra RFB server at port {}", port);
      }
      m_servers.clear();
   }

   bool ExtraRfbServers::startUp(bool asService, RfbClientManager *mgr)
   {
      m_plogwriter->debug("Requested to start up extra RFB servers");

      if (!m_servers.empty())
      {
         m_plogwriter->informationf("Extra RFB servers active, will have to stop them");
         shutDown();
      }

      Conf newConf;
      getConfiguration(&newConf);
      m_effectiveConf = newConf;

      if (newConf.acceptConnections)
      {
         const char *pszBindHost = newConf.loopbackOnly ? "localhost" : "0.0.0.0";

         for (size_t i = 0; i < newConf.extraPorts.count(); i++)
         {
            PortMapping pm = *newConf.extraPorts.at(i);
            PortMappingRect rectangle = pm.getRect();
            int port = pm.getPort();

            m_plogwriter->debug("Starting extra RFB server at port {}", port);

            try
            {
               RfbServer *s = new RfbServer(pszBindHost, m_pconfigurator, port, mgr, asService, m_plogwriter, rectangle);
               m_servers.add(s);
               m_plogwriter->debug("Started extra RFB server at port {}", port);
            }
            catch (::subsystem::Exception &ex)
            {
               m_plogwriter->error("Failed to start extra RFB server: \"{}\"", ex.get_message());
            }
         }
      }

      // If the number of requested port mappings equals to the number of
      // successfully started servers, then everything went fine, return true.
      return newConf.extraPorts.count() == m_servers.size();
   }

   void ExtraRfbServers::getConfiguration(Conf *out)
   {
      // FIXME: Create a sort of configuration accessor with auto-locking, and
      //        do not allow to access the configuration directly, so we would
      //        have to write something like this:
      //        {
      //          ConfigAccessor ca;                         // lock
      //          ::remoting_node::ServerConfig *cfg = ca.getServerConfig();  // get access
      //          someSetting = cfg.getSomeSetting();        // use
      //        }                                            // auto-unlock
      //
      auto pserverconfig = m_pconfigurator->getServerConfig();
      AutoLock l(pserverconfig);

      out->acceptConnections = pserverconfig->isAcceptingRfbConnections();
      out->loopbackOnly = pserverconfig->isOnlyLoopbackConnectionsAllowed();
      out->extraPorts = *pserverconfig->getPortMappingContainer();
   }


} // namespace remoting_node_desktop
 


