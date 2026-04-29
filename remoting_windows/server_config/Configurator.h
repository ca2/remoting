// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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


#include "remoting/remoting/config/SettingsManager.h"

#include "remoting/remoting/node_config/PortMappingContainer.h"
#include "remoting/remoting/node_config/IpAccessControl.h"
#include "remoting/remoting/node_config/ServerConfig.h"
#include "remoting/remoting/node_config/ConfigReloadListener.h"
#include "remoting/remoting/node_config/RegistrySecurityAttributes.h"

#include "subsystem/platform/ListenerContainer.h"


namespace remoting
{


   class Configurator : public ::subsystem::ListenerContainer<ConfigReloadListener *>
   {
   public:


      Configurator();
      ~Configurator() override;


      virtual void initialize_configurator(bool isConfiguringService);

      bool getServiceFlag() { return m_isConfiguringService; }
      void setServiceFlag(bool asService) { m_isConfiguringService = asService; }

      bool load();
      bool save();

      bool isConfigLoadedPartly() { return m_isConfigLoadedPartly; }

      //
      // Get global configurator method
      //

      //static Configurator *getInstance();
      //static void setInstance(Configurator *conf);

      void notifyReload();

      //
      // Protected members read methods
      //

      ::remoting_node::ServerConfig *getServerConfig() { return m_pserverconfig; }

   //private:

      //
      // Serialize and deserialize methods
      //

      bool savePortMappingContainer(::remoting::SettingsManager *psettingsmanager);
      bool loadPortMappingContainer(::remoting::SettingsManager *psettingsmanager, PortMappingContainer *portMapping);

      bool saveIpAccessControlContainer(::remoting::SettingsManager *psettingsmanager);
      bool loadIpAccessControlContainer(::remoting::SettingsManager *psettingsmanager, IpAccessControl *ipContainer);

      bool saveServerConfig(::remoting::SettingsManager *psettingsmanager);
      bool loadServerConfig(::remoting::SettingsManager *psettingsmanager, ::remoting_node::ServerConfig * pserverconfig);
      void updateLogDirPath();

      bool saveQueryConfig(::remoting::SettingsManager *psettingsmanager);
      bool loadQueryConfig(::remoting::SettingsManager *psettingsmanager, ::remoting_node::ServerConfig * pserverconfig);

      bool saveInputHandlingConfig(::remoting::SettingsManager *psettingsmanager);
      bool loadInputHandlingConfig(::remoting::SettingsManager *psettingsmanager, ::remoting_node::ServerConfig * pserverconfig);

      bool saveVideoRegionConfig(::remoting::SettingsManager *psettingsmanager);
      bool loadVideoRegionConfig(::remoting::SettingsManager *psettingsmanager, ::remoting_node::ServerConfig * pserverconfig);

      bool save(::remoting::SettingsManager *psettingsmanager);
      bool load(::remoting::SettingsManager *psettingsmanager);

      bool load(bool forService);
      bool save(bool forService);
   //protected:

      //
      // Server configuration
      //

      ::pointer < ::remoting_node::ServerConfig > m_pserverconfig;

      //
      // Is this flag is set configurator think than application run as service
      //

      bool m_isConfiguringService;
      bool m_isConfigLoadedPartly;

      //
      // Registry security attributes.
      //

      // RegistrySecurityAttributes *m_regSA;

      //
      // Helper members
      //

      bool m_isFirstLoad;

      //
      // Global configurator object
      //

      //static Configurator *s_instance;
      //static lockable_critical_section m_instanceMutex;
   };


} // namespace remoting
 


