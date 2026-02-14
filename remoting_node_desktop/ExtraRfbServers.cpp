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

#include "ExtraRfbServers.h"
#include "server_config_lib/Configurator.h"

ExtraRfbServers::Conf::Conf()
: acceptConnections(false),
  loopbackOnly(false),
  extraPorts()
{
}

ExtraRfbServers::Conf::Conf(const Conf &other)
: acceptConnections(other.acceptConnections),
  loopbackOnly(other.loopbackOnly),
  extraPorts(other.extraPorts)
{
}

ExtraRfbServers::Conf &
ExtraRfbServers::Conf::operator=(const Conf &other)
{
  acceptConnections = other.acceptConnections;
  loopbackOnly = other.loopbackOnly;
  extraPorts = other.extraPorts;
  return *this;
}

bool ExtraRfbServers::Conf::equals(const Conf *other)
{
  return (acceptConnections == other->acceptConnections &&
          loopbackOnly == other->loopbackOnly &&
          extraPorts.equals(&other->extraPorts));
}

ExtraRfbServers::ExtraRfbServers(LogWriter *log)
: m_servers(),
  m_effectiveConf(),
  m_log(log)
{
}

ExtraRfbServers::~ExtraRfbServers()
{
  try {
    shutDown();
  } catch (...) { }
}

bool ExtraRfbServers::reload(bool asService, RfbClientManager *mgr)
{
  m_log->detail("Considering to reload extra RFB servers");

  Conf newConf;
  getConfiguration(&newConf);
  bool noConfigChanges = newConf.equals(&m_effectiveConf);
  bool enoughServers = (newConf.extraPorts.count() == m_servers.size());
  m_log->detail("Same Extra Ports configuration = {}, enough servers = {}",
              (int)noConfigChanges, (int)enoughServers);

  if (noConfigChanges && enoughServers) {
    return true; // no work needed, no errors encountered
  }

  // Either configuration was actually changed, or our number of actually
  // running servers does not match the configuration. In either case,
  // restart the servers.
  m_log->message("Need to reconfigure extra RFB servers");
  shutDown();
  return startUp(asService, mgr);
}

void ExtraRfbServers::shutDown()
{
  m_log->detail("Requested to shut down extra RFB servers");

  ::list<RfbServer *>::const_iterator i;
  for (i = m_servers.begin(); i != m_servers.end(); i++) {
    int port = (*i)->getBindPort();
    m_log->detail("Stopping extra RFB server at port {}", port);
    delete *i;
    m_log->message("Stopped extra RFB server at port {}", port);
  }
  m_servers.clear();
}

bool ExtraRfbServers::startUp(bool asService, RfbClientManager *mgr)
{
  m_log->detail("Requested to start up extra RFB servers");

  if (!m_servers.empty()) {
    m_log->interror("Extra RFB servers active, will have to stop them");
    shutDown();
  }

  Conf newConf;
  getConfiguration(&newConf);
  m_effectiveConf = newConf;

  if (newConf.acceptConnections) {
    const ::scoped_string & scopedstrbindHost =
      newConf.loopbackOnly ? "localhost") : _T("0.0.0.0";

    for (size_t i = 0; i < newConf.extraPorts.count(); i++) {
      PortMapping pm = *newConf.extraPorts.at(i);
      PortMappingRect rect = pm;
      int port = pm.getPort();

      m_log->detail("Starting extra RFB server at port {}", port);

      try {
        RfbServer *s = new RfbServer(bindHost, port, mgr, asService, m_log, &rect);
        m_servers.add(s);
        m_log->message("Started extra RFB server at port {}", port);
      } catch (::remoting::Exception &ex) {
        m_log->error("Failed to start extra RFB server: \"{}\"",
                   ex.getMessage());
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
  //          ServerConfig *cfg = ca.getServerConfig();  // get access
  //          someSetting = cfg.getSomeSetting();        // use
  //        }                                            // auto-unlock
  //
  ServerConfig *config = Configurator::getInstance()->getServerConfig();
  AutoLock l(config);

  out->acceptConnections = config->isAcceptingRfbConnections();
  out->loopbackOnly = config->isOnlyLoopbackConnectionsAllowed();
  out->extraPorts = *config->getPortMappingContainer();
}
