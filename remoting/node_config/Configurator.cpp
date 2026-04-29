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
#include "framework.h"
#include "subsystem/node/OperatingSystem.h"
//#include "remoting/remoting/node_user_config/TvnLogFilename.h"
#include "remoting/remoting/config/RegistrySettingsManager.h"

#include "subsystem/platform/Registry.h"
#include "subsystem/platform/RegistryKey.h"

#include "Configurator.h"
//#include "remoting/node_desktop/NamingDefs.h"


namespace remoting
{

   //Configurator *Configurator::s_instance = NULL;
   //lockable_critical_section Configurator::m_instanceMutex;

   Configurator::Configurator() :
       m_isConfiguringService(false), m_isConfigLoadedPartly(false), m_isFirstLoad(true)
   //, m_regSA(0)
   {
   }

   Configurator::~Configurator()
   {
      // if (m_regSA != 0) delete m_regSA;
   }

   void Configurator::initialize_configurator(bool isConfiguringService) //:
       //m_isConfiguringService(isConfiguringService), m_isConfigLoadedPartly(false), m_isFirstLoad(true)
   //, m_regSA(0)
   {
      // critical_section_lock al(&m_instanceMutex);
      // if (s_instance != 0)
      // {
      //    throw ::subsystem::Exception("Configurator instance already exists");
      // }
      // s_instance = this;
      // try
      // {
      //    // m_regSA = new RegistrySecurityAttributes();
      // }
      // catch (...)
      // {
      //    // TODO: Place exception handler here.
      // }
      m_isConfiguringService=isConfiguringService;
   }

   // Configurator *Configurator::getInstance()
   // {
   //    critical_section_lock al(&m_instanceMutex);
   //    _ASSERT(s_instance != NULL);
   //    return s_instance;
   // }
   //
   // void Configurator::setInstance(Configurator *conf) { s_instance = conf; }

   void Configurator::notifyReload()
   {
      critical_section_lock l(&m_listeners);

      for (size_t i = 0; i < m_listeners.size(); i++)
      {
         m_listeners.at(i)->onConfigReload(getServerConfig());
      }
   }

   bool Configurator::load() { return load(m_isConfiguringService); }

   bool Configurator::save() { return save(m_isConfiguringService); }

   bool Configurator::load(bool forService)
   {
      bool isOk = false;

      //::subsystem::registry rootKey = forService ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
      ::subsystem::RegistryKey *pregistrykey = nullptr;
      if (forService)
      {

         pregistrykey = MainSubsystem().Registry().getLocalMachineKey();
      }
      else
      {

         pregistrykey = MainSubsystem().Registry().getCurrentUserKey();
      }
      // SECURITY_ATTRIBUTES *sa = 0;
      // if (forService && m_regSA != 0) {
      //   sa = m_regSA->getServiceSA();
      // }
      // RegistrySettingsManager sm(rootKey, RegistryPaths::SERVER_PATH, sa);

      ::remoting::RegistrySettingsManager sm(pregistrykey, "Software/remoting/server");

      isOk = load(&sm);

      notifyReload();

      return isOk;
   }

   bool Configurator::save(bool forService)
   {
      bool isOk = false;

      //::subsystem::registry rootKey = forService ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
      ::subsystem::RegistryKey *pregistrykey = nullptr;
      if (forService)
      {

         pregistrykey = MainSubsystem().Registry().getLocalMachineKey();
      }
      else
      {

         pregistrykey = MainSubsystem().Registry().getCurrentUserKey();
      }


      // SECURITY_ATTRIBUTES *sa = 0;
      // if (forService && m_regSA != 0) {
      //   sa = m_regSA->getServiceSA();
      // }
      // RegistrySettingsManager sm(rootKey, RegistryPaths::SERVER_PATH, sa);

      // isOk = save(&sm);

      return isOk;
   }

   bool Configurator::save(::remoting::SettingsManager *sm)
   {
      bool saveResult = true;
      if (!savePortMappingContainer(sm))
      {
         saveResult = false;
      }
      if (!saveQueryConfig(sm))
      {
         saveResult = false;
      }
      if (!saveInputHandlingConfig(sm))
      {
         saveResult = false;
      }
      if (!saveIpAccessControlContainer(sm))
      {
         saveResult = false;
      }
      if (!saveServerConfig(sm))
      {
         saveResult = false;
      }
      if (!saveVideoRegionConfig(sm))
      {
         saveResult = false;
      }
      return saveResult;
   }

   bool Configurator::load(::remoting::SettingsManager *sm)
   {
      bool loadResult = true;

      {
         AutoLock l(m_pserverconfig.m_p);

         if (!loadPortMappingContainer(sm, m_pserverconfig->getPortMappingContainer()))
         {
            loadResult = false;
         }
      }

      if (!loadQueryConfig(sm, m_pserverconfig))
      {
         loadResult = false;
      }
      if (!loadInputHandlingConfig(sm, m_pserverconfig))
      {
         loadResult = false;
      }

      {
         AutoLock l(m_pserverconfig.m_p);

         if (!loadIpAccessControlContainer(sm, m_pserverconfig->getAccessControl()))
         {
            loadResult = false;
         }
      }

      if (!loadServerConfig(sm, m_pserverconfig))
      {
         loadResult = false;
      }
      if (!loadVideoRegionConfig(sm, m_pserverconfig))
      {
         loadResult = false;
      }

      m_isFirstLoad = false;

      return loadResult;
   }

   bool Configurator::savePortMappingContainer(::remoting::SettingsManager *sm)
   {
      bool saveResult = true;

      //
      // Get port mappings from server config
      //

      AutoLock l(m_pserverconfig.m_p);

      PortMappingContainer *portMappings = m_pserverconfig->getPortMappingContainer();

      size_t count = portMappings->count();
      ::string portMappingsString;
      ::string portMappingString;

      //
      // Create string to serialize
      //

      portMappingsString = "";
      for (size_t i = 0; i < count; i++)
      {
         const PortMapping *portMapping = portMappings->at(i);
         portMapping->toString(portMappingString);
         portMappingsString += portMappingString;
         if (i != count - 1)
         {
            portMappingsString += ",";
         }
      }

      //
      // Save port mappings
      //

      if (!sm->setString("ExtraPorts", portMappingsString))
      {
         saveResult = false;
      }
      return saveResult;
   }

   bool Configurator::loadPortMappingContainer(::remoting::SettingsManager *sm, PortMappingContainer *portMapping)
   {
      bool wasError = false;

      portMapping->removeAll();

      ::string extraPorts;

      if (!sm->getString("ExtraPorts", extraPorts))
      {
         return false;
      }

      size_t count = 0;
      ::string_array chunks;

      chunks.explode(",", extraPorts);
      // extraPorts.split(",", NULL, &count);
      if (chunks.has_element())
      {
         //::string_array chunks(count);
         // extraPorts.split(",", &chunks.front(), &count);

         PortMapping mapping;

         for (size_t i = 0; i < count; i++)
         {
            if (PortMapping::parse(chunks[i], &mapping))
            {
               portMapping->pushBack(mapping);
            }
            else
            {
               wasError = true;
            }
         }
      }

      return !wasError;
   }

   bool Configurator::saveQueryConfig(::remoting::SettingsManager *sm)
   {
      bool saveResult = true;
      if (!sm->setUINT("QueryTimeout", m_pserverconfig->getQueryTimeout()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("QueryAcceptOnTimeout", m_pserverconfig->isDefaultActionAccept()))
      {
         saveResult = false;
      }
      return saveResult;
   }

   bool Configurator::loadQueryConfig(::remoting::SettingsManager *sm, ServerConfig * pserverconfig)
   {
      bool loadResult = true;
      unsigned int uintValue;
      bool boolValue;
      if (!sm->getUINT("QueryTimeout", &uintValue))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setQueryTimeout(uintValue);
      }
      if (!sm->getBoolean("QueryAcceptOnTimeout", &boolValue))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setDefaultActionToAccept(boolValue);
      }
      return loadResult;
   }

   bool Configurator::saveInputHandlingConfig(::remoting::SettingsManager *sm)
   {
      bool saveResult = true;
      if (!sm->setUINT("LocalInputPriorityTimeout", m_pserverconfig->getLocalInputPriorityTimeout()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("LocalInputPriority", m_pserverconfig->isLocalInputPriorityEnabled()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("BlockRemoteInput", m_pserverconfig->isBlockingRemoteInput()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("BlockLocalInput", m_pserverconfig->isBlockingLocalInput()))
      {
         saveResult = false;
      }
      return saveResult;
   }

   bool Configurator::loadInputHandlingConfig(::remoting::SettingsManager *sm, ServerConfig * pserverconfig)
   {
      bool loadResult = true;

      //
      // Temporary variables
      //

      bool boolVal = false;
      unsigned int uintVal = 0;

      if (!sm->getUINT("LocalInputPriorityTimeout", &uintVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         pserverconfig->setLocalInputPriorityTimeout(uintVal);
      }
      if (!sm->getBoolean("LocalInputPriority", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         pserverconfig->setLocalInputPriority(boolVal);
      }
      if (!sm->getBoolean("BlockRemoteInput", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         pserverconfig->blockRemoteInput(boolVal);
      }
      if (!sm->getBoolean("BlockLocalInput", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         pserverconfig->blockLocalInput(boolVal);
      }

      return loadResult;
   }

   bool Configurator::saveVideoRegionConfig(::remoting::SettingsManager *sm)
   {
      bool saveResult = true;

      ::string buffer;
      ::string_array *videoClasses = m_pserverconfig->getVideoClassNames();
      size_t size = videoClasses->size();
      ::int_rectangle_array_base *rectangleaVideo = m_pserverconfig->getVideoRects();

      AutoLock l(m_pserverconfig.m_p);
      buffer = "";
      for (size_t i = 0; i < size; i++)
      {
         buffer += videoClasses->at(i);
         if (i != size - 1)
         {
            buffer += "\n";
         }
      }
      if (!sm->setString("VideoClasses", buffer))
      {
         saveResult = false;
      }

      size = rectangleaVideo->size();
      buffer = "";
      for (size_t i = 0; i < size; i++)
      {
         ::string s;
         ::remoting::RectSerializer::toString(rectangleaVideo->at(i), s);
         buffer += s;
         if (i != size - 1)
         {
            buffer += "\n";
         }
      }
      if (!sm->setString("VideoRects", buffer))
      {
         saveResult = false;
      }
      return saveResult;
   }

   bool Configurator::loadVideoRegionConfig(::remoting::SettingsManager *sm, ServerConfig * pserverconfig)
   {
      bool loadResult = true;

      ::string_array *videoClasses = m_pserverconfig->getVideoClassNames();
      ::int_rectangle_array_base *rectangleaVideo = m_pserverconfig->getVideoRects();

      // Lock configuration
      AutoLock l(m_pserverconfig.m_p);

      //
      // Delete old video classes entries
      //

      videoClasses->clear();

      //
      // Try to load.
      //

      ::string storage;

      if (!sm->getString("VideoClasses", storage))
      {
         loadResult = false;
      }

      //
      // Split.
      //

      size_t count = 0;
      ::string_array chunks;
      chunks.explode("\n\t ,;", storage);
      if (chunks.has_element())
      {

         //    storage.split("\n\t ,;", &chunks.front(), &count);

         for (size_t i = 0; i < count; i++)
         {
            if (!chunks[i].is_empty())
            {
               videoClasses->add(chunks[i]);
            }
         }
      }

      // Clear storage content
      storage = "";

      //
      // Delete old video rectanglea entries
      //

      rectangleaVideo->clear();

      //
      // Try to load.
      //

      if (!sm->getString("VideoRects", storage))
      {
         loadResult = false;
      }

      //
      // Split.
      //

      count = 0;
      //::string_array chunks;
      chunks.clear();

      chunks.explode("\n\t ,;", storage);
      // storage.split("\n\t ,;", NULL, &count);
      if (chunks.has_element())
      {
         //::string_array chunks(count);
         // storage.split(, &chunks.front(), &count);

         for (size_t i = 0; i < count; i++)
         {
            if (!chunks[i].is_empty())
            {
               rectangleaVideo->add(::remoting::RectSerializer::toRect(chunks[i]));
            }
         }
      }
      return loadResult;
   }

   bool Configurator::saveIpAccessControlContainer(::remoting::SettingsManager *storage)
   {
      AutoLock l(m_pserverconfig.m_p);

      // Get rules container
      IpAccessControl *rules = m_pserverconfig->getAccessControl();
      // Remember rules count
      size_t rulesCount = rules->size();
      // 1 rule can contain 34 character max
      size_t maxStringBufferLength = 34 * 2 * rulesCount;
      // Buffer that we need to write to storage
      ::string buffer("");
      // Variable to save temporary result from toString method
      ::string ruleString;

      // Generate rules string
      for (size_t i = 0; i < rulesCount; i++)
      {
         IpAccessRule *rule = rules->at(i);
         // Get rule as string
         rule->toString(ruleString);
         // Add it to result buffer
         buffer += ruleString;
         // Add delimiter if we need it
         if (i != rulesCount - 1)
            buffer += ",";
      }
      if (!storage->setString("IpAccessControl", buffer))
      {
         return false;
      }
      return true;
   }

   bool Configurator::loadIpAccessControlContainer(::remoting::SettingsManager *sm, IpAccessControl *rules)
   {
      bool wasError = false;
      rules->clear();

      ::string storage;
      if (!sm->getString("IpAccessControl", storage))
      {
         return false;
      }
      else
      {
         size_t maxBufSize = storage.length() + 1;
         ::memory ipacStringBuffer;

         ipacStringBuffer.set_size(maxBufSize + 1);
         auto p = (char *)ipacStringBuffer.data();
         strcpy(p, storage);
         //_tcscpy_s(&ipacStringBuffer.front(), maxBufSize, storage);
         auto pch = strtok(p, ",");
         while (pch != NULL)
         {
            if (IpAccessRule::parse(pch, NULL))
            {
               IpAccessRule *rule = new IpAccessRule();
               IpAccessRule::parse(pch, rule);
               rules->add(rule);
            }
            else
            {
               wasError = true;
            }
            pch = strtok(NULL, ",");
         } // while
      } // else
      return !wasError;
   }

   bool Configurator::saveServerConfig(::remoting::SettingsManager *sm)
   {
      bool saveResult = true;
      if (!sm->setUINT("RfbPort", m_pserverconfig->getRfbPort()))
      {
         saveResult = false;
      }
      if (!sm->setUINT("HttpPort", m_pserverconfig->getHttpPort()))
      {
         saveResult = false;
      }
      if (!sm->setUINT("DisconnectAction", (unsigned int)m_pserverconfig->getDisconnectAction()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("AcceptRfbConnections", m_pserverconfig->isAcceptingRfbConnections()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("UseVncAuthentication", m_pserverconfig->isUsingAuthentication()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("UseControlAuthentication", m_pserverconfig->isControlAuthEnabled()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("RepeatControlAuthentication", m_pserverconfig->getControlAuthAlwaysChecking()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("LoopbackOnly", m_pserverconfig->isOnlyLoopbackConnectionsAllowed()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("AcceptHttpConnections", m_pserverconfig->isAcceptingHttpConnections()))
      {
         saveResult = false;
      }
      if (!sm->setUINT("LogLevel", (unsigned int)m_pserverconfig->getLogLevel()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("EnableFileTransfers", m_pserverconfig->isFileTransfersEnabled()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("RemoveWallpaper", m_pserverconfig->isRemovingDesktopWallpaperEnabled()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("UseD3D", m_pserverconfig->getD3DIsAllowed()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("UseMirrorDriver", m_pserverconfig->getMirrorIsAllowed()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("EnableUrlParams", m_pserverconfig->isAppletParamInUrlEnabled()))
      {
         saveResult = false;
      }
      if (m_pserverconfig->hasPrimaryPassword())
      {
         unsigned char password[ServerConfig::VNC_PASSWORD_SIZE];

         m_pserverconfig->getPrimaryPassword(&password[0]);

         if (!sm->setBinaryData("Password", &password[0], ServerConfig::VNC_PASSWORD_SIZE))
         {
            saveResult = false;
         }
      }
      else
      {
         sm->deleteKey("Password");
      }
      if (m_pserverconfig->hasReadOnlyPassword())
      {
         unsigned char password[ServerConfig::VNC_PASSWORD_SIZE];

         m_pserverconfig->getReadOnlyPassword(&password[0]);

         if (!sm->setBinaryData("PasswordViewOnly", &password[0], ServerConfig::VNC_PASSWORD_SIZE))
         {
            saveResult = false;
         }
      }
      else
      {
         sm->deleteKey("PasswordViewOnly");
      }
      if (m_pserverconfig->hasControlPassword())
      {
         unsigned char password[ServerConfig::VNC_PASSWORD_SIZE];

         m_pserverconfig->getControlPassword(&password[0]);

         if (!sm->setBinaryData("ControlPassword", &password[0], ServerConfig::VNC_PASSWORD_SIZE))
         {
            saveResult = false;
         }
      }
      else
      {
         sm->deleteKey("ControlPassword");
      }
      if (!sm->setBoolean("AlwaysShared", m_pserverconfig->isAlwaysShared()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("NeverShared", m_pserverconfig->isNeverShared()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("DisconnectClients", m_pserverconfig->isDisconnectingExistingClients()))
      {
         saveResult = false;
      }
      if (!sm->setUINT("PollingInterval", m_pserverconfig->getPollingInterval()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("AllowLoopback", m_pserverconfig->isLoopbackConnectionsAllowed()))
      {
         saveResult = false;
      }
      if (!sm->setUINT("VideoRecognitionInterval", m_pserverconfig->getVideoRecognitionInterval()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("GrabTransparentWindows", m_pserverconfig->getGrabTransparentWindowsFlag()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("SaveLogToAllUsersPath", m_pserverconfig->isSaveLogToAllUsersPathFlagEnabled()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("RunControlInterface", m_pserverconfig->getShowTrayIconFlag()))
      {
         saveResult = false;
      }
      if (!sm->setBoolean("ConnectToRdp", m_pserverconfig->getConnectToRdpFlag()))
      {
         saveResult = false;
      }
      if (!sm->setUINT("IdleTimeout", (unsigned int)m_pserverconfig->getIdleTimeout()))
      {
         saveResult = false;
      }
      return saveResult;
   }

   bool Configurator::loadServerConfig(::remoting::SettingsManager *sm, ServerConfig * pserverconfig)
   {
      bool loadResult = true;

      //
      // Temporary variables
      //

      bool boolVal;
      unsigned int uintVal;

      if (!sm->getUINT("RfbPort", &uintVal))
      {
         loadResult = false;
      }
      else
      {
         m_pserverconfig->setRfbPort(uintVal);
      }
      if (!sm->getUINT("HttpPort", &uintVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setHttpPort(uintVal);
      }
      if (!sm->getUINT("DisconnectAction", &uintVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setDisconnectAction((ServerConfig::DisconnectAction)uintVal);
      }
      if (!sm->getBoolean("AcceptRfbConnections", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->acceptRfbConnections(boolVal);
      }
      if (!sm->getBoolean("UseVncAuthentication", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->useAuthentication(boolVal);
      }
      if (!sm->getBoolean("UseControlAuthentication", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->useControlAuth(boolVal);
      }
      if (!sm->getBoolean("RepeatControlAuthentication", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setControlAuthAlwaysChecking(boolVal);
      }
      if (!sm->getBoolean("LoopbackOnly", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->acceptOnlyLoopbackConnections(boolVal);
      }
      if (!sm->getBoolean("AcceptHttpConnections", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->acceptHttpConnections(boolVal);
      }
      if (!sm->getUINT("LogLevel", &uintVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setLogLevel(uintVal);
      }
      if (!sm->getBoolean("EnableFileTransfers", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->enableFileTransfers(boolVal);
      }
      if (!sm->getBoolean("RemoveWallpaper", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->enableRemovingDesktopWallpaper(boolVal);
      }
      if (!sm->getBoolean("UseD3D", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setD3DAllowing(boolVal);
      }
      if (!sm->getBoolean("UseMirrorDriver", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setMirrorAllowing(boolVal);
      }
      if (!sm->getBoolean("EnableUrlParams", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->enableAppletParamInUrl(boolVal);
      }

      memsize passSize = 8;
      unsigned char buffer[ServerConfig::VNC_PASSWORD_SIZE] = {0};

      if (!sm->getBinaryData("Password", (void *)&buffer, &passSize))
      {
         loadResult = false;
         m_pserverconfig->deletePrimaryPassword();
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setPrimaryPassword(&buffer[0]);
      }
      passSize = 8;
      if (!sm->getBinaryData("PasswordViewOnly", (void *)&buffer, &passSize))
      {
         loadResult = false;
         m_pserverconfig->deleteReadOnlyPassword();
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setReadOnlyPassword(&buffer[0]);
      }
      passSize = 8;
      if (!sm->getBinaryData("ControlPassword", (void *)&buffer, &passSize))
      {
         loadResult = false;
         m_pserverconfig->deleteControlPassword();
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setControlPassword(&buffer[0]);
      }

      if (!sm->getBoolean("AlwaysShared", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setAlwaysShared(boolVal);
      }
      if (!sm->getBoolean("NeverShared", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setNeverShared(boolVal);
      }
      if (!sm->getBoolean("DisconnectClients", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->disconnectExistingClients(boolVal);
      }
      if (!sm->getUINT("PollingInterval", &uintVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setPollingInterval(uintVal);
      }
      if (!sm->getBoolean("AllowLoopback", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->allowLoopbackConnections(boolVal);
      }
      if (!sm->getUINT("VideoRecognitionInterval", &uintVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setVideoRecognitionInterval(uintVal);
      }
      if (!sm->getUINT("IdleTimeout", &uintVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setIdleTimeout((int)uintVal);
      }
      if (!sm->getBoolean("GrabTransparentWindows", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setGrabTransparentWindowsFlag(boolVal);
      }
      if (!sm->getBoolean("SaveLogToAllUsersPath", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->saveLogToAllUsersPath(boolVal);
      }
      if (!sm->getBoolean("RunControlInterface", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setShowTrayIconFlag(boolVal);
      }

      if (!sm->getBoolean("ConnectToRdp", &boolVal))
      {
         loadResult = false;
      }
      else
      {
         m_isConfigLoadedPartly = true;
         m_pserverconfig->setConnectToRdpFlag(boolVal);
      }

      updateLogDirPath();
      return loadResult;
   }

   void Configurator::updateLogDirPath()
   {
      //::string pathToLogDirectory;
      // TvnLogFilename::queryLogFileDirectory(m_isConfiguringService,
      //   m_pserverconfig->isSaveLogToAllUsersPathFlagEnabled(),
      //   &pathToLogDirectory);
      // m_pserverconfig->setLogFileDir(pathToLogDirectory);
   }


} // namespace remoting
 


