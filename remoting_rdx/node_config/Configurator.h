// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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


#include "remoting/remoting_rfb/config/SettingsManager.h"

#include "remoting/remoting_rfb/node_config/PortMappingContainer.h"
#include "remoting/remoting_rfb/node_config/IpAccessControl.h"
#include "remoting/remoting_rfb/node_config/ServerConfig.h"
#include "remoting/remoting_rfb/node_config/ConfigReloadListener.h"
#include "remoting/remoting_rfb/node_config/RegistrySecurityAttributes.h"

#include "subsystem/platform/ListenerContainer.h"


namespace remoting_rfb_node
{


   class CLASS_DECL_REMOTING_RDX Configurator : public ::subsystem::ListenerContainer<ConfigReloadListener *>
   {
   public:
      //protected:

      //
      // Server configuration
      //

      ::pointer < ServerConfig > m_pserverconfig;

      //
      // Is this flag is set configurator think than application onThreadMain as service
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
      //static void setInstance(::remoting_rfb_node::Configurator *conf);

      void notifyReload();

      //
      // Protected members read methods
      //

      ServerConfig *getServerConfig() { return m_pserverconfig; }

   //private:

      //
      // Serialize and deserialize methods
      //

      bool savePortMappingContainer(::remoting_rfb::SettingsManager *psettingsmanager);
      bool loadPortMappingContainer(::remoting_rfb::SettingsManager *psettingsmanager, PortMappingContainer *portMapping);

      bool saveIpAccessControlContainer(::remoting_rfb::SettingsManager *psettingsmanager);
      bool loadIpAccessControlContainer(::remoting_rfb::SettingsManager *psettingsmanager, IpAccessControl *ipContainer);

      bool saveServerConfig(::remoting_rfb::SettingsManager *psettingsmanager);
      bool loadServerConfig(::remoting_rfb::SettingsManager *psettingsmanager, ServerConfig * pserverconfig);
      void updateLogDirPath();

      bool saveQueryConfig(::remoting_rfb::SettingsManager *psettingsmanager);
      bool loadQueryConfig(::remoting_rfb::SettingsManager *psettingsmanager, ServerConfig * pserverconfig);

      bool saveInputHandlingConfig(::remoting_rfb::SettingsManager *psettingsmanager);
      bool loadInputHandlingConfig(::remoting_rfb::SettingsManager *psettingsmanager, ServerConfig * pserverconfig);

      bool saveVideoRegionConfig(::remoting_rfb::SettingsManager *psettingsmanager);
      bool loadVideoRegionConfig(::remoting_rfb::SettingsManager *psettingsmanager, ServerConfig * pserverconfig);

      bool save(::remoting_rfb::SettingsManager *psettingsmanager);
      bool load(::remoting_rfb::SettingsManager *psettingsmanager);

      bool load(bool forService);
      bool save(bool forService);

   };


} // namespace remoting_rfb_node
 


