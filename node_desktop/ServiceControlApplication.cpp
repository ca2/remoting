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
#include "ServiceControlApplication.h"
#include "ServiceControlCommandLine.h"

#include "subsystem/platform/ResourceLoader.h"
#include "subsystem/platform/StringTable.h"
#include "remoting/node_desktop/NamingDefs.h"

#include "remoting/node_desktop/resource.h"

#include "WsConfigRunner.h"
#include "ServerHelp.h"

#include "remoting/control_desktop/ControlCommandLine.h"

#include "subsystem/platform/Registry.h"
#include "subsystem/platform/RegistryKey.h"
#include "subsystem/node/OperatingSystem.h"
#include "subsystem/node/Shell.h"
#include "subsystem/node/ServiceControlManagerClient.h"
#include "subsystem/platform/CommandLineArguments.h"
#include "resource.h"

namespace remoting_node_desktop
{
   ServiceControlApplication::ServiceControlApplication(HINSTANCE hInstance,
                                                        const ::scoped_string & scopedstrwindowClassName,
                                                        const ::scoped_string & scopedstrCommandLine)
   //: WindowsApplication(hInstance, windowClassName),
   :  m_commandLine(scopedstrCommandLine)
   {
      initialize_operating_system_application();
   }

   ServiceControlApplication::~ServiceControlApplication()
   {
   }

   void ServiceControlApplication::run()
   {
      // FIXME: Make these constants of the base class, or better make the
      //        function return bool and make the caller care about the proper
      //        return values for WinMain().
      const int RET_OK = 0;
      const int RET_ERR = 1;

      // FIXME: Make this a member variable, parse in a separate function.
      ServiceControlCommandLine cmdLine;
      try {
         //WinCommandLineArgs cmdArgs(m_commandLine);

         auto pcommandlinearguments = MainSubsystem().getCommandLineArguments(m_commandLine);
         cmdLine.parse(pcommandlinearguments);
      } catch (::subsystem::Exception &) {
         ServerHelp::showUsage();
         setExitCode(RET_ERR);
         return;
         //return RET_ERR;
      }

      bool success = false;

      // FIXME: Is the call to IsUserAnAdmin() a correct check for being able to
      //        install/remove/start/stop system services?
      if (!MainSubsystem().OperatingSystem().isUserAnAdmin() && !cmdLine.beSilent() && !cmdLine.dontElevate()) {
         // The privileges are insufficient and there were no -silent/-dontelevate
         // options so we can request privilege elevation and run another
         // "elevated" instance of the same program, with additional -dontelevate
         // option in the command line.
         try {
            runElevatedInstance();
            success = true;
         } catch (::subsystem::SystemException &sysEx) {
            if (sysEx.getErrorCode() != ERROR_CANCELLED) {
               reportError(&cmdLine, sysEx.get_message());
            }
         }
      } else {
         // Do the work in current instance, do not request privilege elevation.
         try {
            executeCommand(&cmdLine);
            success = true;
         } catch (::subsystem::ServiceControlManagerClientException &scmEx) {
            reportError(&cmdLine, &scmEx);
         } catch (::subsystem::SystemException &servEx) {
            reportError(&cmdLine, &servEx);
         } catch (::subsystem::Exception &ex) {
            _ASSERT(false);
            reportError(&cmdLine, ex.get_message());
         }
      }

      // After trying to start the service, regardless of the result, run the
      // control interface with non-elevated privileges. Make sure to skip this
      // step if there was a -dontelevate option (so we are a child process).
      if (cmdLine.startRequested() && !cmdLine.dontElevate()) 
      {

         try 
         {

            // FIXME: WsConfigRunner is a Thread, so the work will be made in a
            //        newly created thread and thus we are not aware of the result.
            //        Are there any reasons why we cannot do that synchronously?
            WsConfigRunner tvncontrol;
            
            tvncontrol.initialize_ws_config_runner(nullptr, true);

         } 
         catch (...) 
         {
         
         }

      }

      setExitCode(success ? RET_OK : RET_ERR);

   }


   void ServiceControlApplication::runElevatedInstance()
   {

      ::string executablePath;
      executablePath = MainSubsystem().OperatingSystem().getCurrentModulePath();

      ::string commandLine;
      commandLine.format("{} {}", m_commandLine,
                         ServiceControlCommandLine::DONT_ELEVATE);

      MainSubsystem().Shell().runAsAdmin(executablePath, commandLine);
   }


   void ServiceControlApplication::executeCommand(const ServiceControlCommandLine *cmdLine)
   {
      if (cmdLine->installationRequested()) {
         Service::install();
         setTvnControlStartEntry();
         reportSuccess(cmdLine);
      } else if (cmdLine->removalRequested()) {
         Service::remove();
         removeTvnControlStartEntry();
         reportSuccess(cmdLine);
      } else if (cmdLine->reinstallRequested()) {
         Service::reinstall();
         setTvnControlStartEntry();
         reportSuccess(cmdLine);
      } else if (cmdLine->startRequested()) {
         Service::start(true);
      } else if (cmdLine->stopRequested()) {
         Service::stop(true);
      }
   }

   void ServiceControlApplication::setTvnControlStartEntry()
   {
      // Prepare tvncontrol start command.
      ::string executablePath;
      executablePath = MainSubsystem().OperatingSystem().getCurrentModulePath();
      ::string pathToTvnControl;
      pathToTvnControl.formatf("\"{}\" {} {}",
                              executablePath,
                              ControlCommandLine::CONTROL_SERVICE,
                              ControlCommandLine::SLAVE_MODE);

      // Write registry entry.
      ::subsystem::RegistryKey runKey(MainSubsystem().Registry().getLocalMachineKey(),
                         "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                         false);
      runKey.setValueAsString(ServiceNames::TVNCONTROL_START_REGISTRY_ENTRY_NAME,
                              pathToTvnControl);
   }

   void ServiceControlApplication::removeTvnControlStartEntry()
   {
      ::subsystem::RegistryKey runKey(MainSubsystem().Registry().getLocalMachineKey(),
                         "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                         false);
      runKey.deleteValue(ServiceNames::TVNCONTROL_START_REGISTRY_ENTRY_NAME);
   }

   void ServiceControlApplication::reportError(const ServiceControlCommandLine *cmdLine,
                                               const ::subsystem::ServiceControlManagerClientException *ex)
   {
      ::string errorMessage;

      switch (ex->getServiceControlManagerErrorCode()) {
         case ::subsystem::ServiceControlManagerClientException::ERROR_ALREADY_RUNNING:
            errorMessage= MainSubsystem().StringTable().getString(IDS_SERVICE_ALREADY_RUNNING);
            break;
         case ::subsystem::ServiceControlManagerClientException::ERROR_ALREADY_STOPPED:
            errorMessage= MainSubsystem().StringTable().getString(IDS_SERVICE_ALREADY_STOPPED);
            break;
         case ::subsystem::ServiceControlManagerClientException::ERROR_START_TIMEOUT:
            errorMessage= MainSubsystem().StringTable().getString(IDS_SERVICE_START_TIMEOUT);
            break;
         case ::subsystem::ServiceControlManagerClientException::ERROR_STOP_TIMEOUT:
            errorMessage= MainSubsystem().StringTable().getString(IDS_SERVICE_STOP_TIMEOUT);
            break;
         default:
            errorMessage= ex->get_message();
      }

      reportError(cmdLine, errorMessage);
   }

   void ServiceControlApplication::reportError(const ServiceControlCommandLine *cmdLine,
                                               const ::subsystem::SystemException *ex)
   {
      ::string errorMessage;

      switch (ex->getErrorCode()) {
         case ERROR_SERVICE_DOES_NOT_EXIST:
            errorMessage= MainSubsystem().StringTable().getString(IDS_1060_ERROR_DESCRIPTION);
            break;
         case ERROR_SERVICE_EXISTS:
            errorMessage= MainSubsystem().StringTable().getString(IDS_1073_ERROR_DESCRIPTION);
            break;
         default:
            errorMessage= ex->get_message();
      }

      reportError(cmdLine, errorMessage);
   }

   void ServiceControlApplication::reportError(const ServiceControlCommandLine *cmdLine,
                                               const ::scoped_string & scopedstrErrorMessage)
   {
      unsigned int stringId = 0;

      if (cmdLine->installationRequested()) {
         stringId = IDS_FAILED_TO_INSTALL_SERVICE_FORMAT;
      } else if (cmdLine->removalRequested()) {
         stringId = IDS_FAILED_TO_REMOVE_SERVICE_FORMAT;
      } else if (cmdLine->reinstallRequested()) {
         stringId = IDS_FAILED_TO_INSTALL_SERVICE_FORMAT;
      } else if (cmdLine->startRequested()) {
         stringId = IDS_FAILED_TO_START_SERVICE_FORMAT;
      } else if (cmdLine->stopRequested()) {
         stringId = IDS_FAILED_TO_STOP_SERVICE_FORMAT;
      } else {
         _ASSERT(false);
         return;
      }

      if (!cmdLine->beSilent()) {

         auto strCaption = MainSubsystem().StringTable().getString(IDS_MBC_TVNSERVER);
         ::string text;
         text.formatf(MainSubsystem().StringTable().getString(stringId), scopedstrErrorMessage);
         MainSubsystem().message_box({}, text, strCaption, ::user::e_message_box_ok | MB_ICONERROR);
      }
   }

   void ServiceControlApplication::reportSuccess(const ServiceControlCommandLine *cmdLine)
   {
      unsigned int stringId = 0;

      if (cmdLine->installationRequested()) {
         stringId = IDS_SERVICE_INSTALLED;
      } else if (cmdLine->removalRequested()) {
         stringId = IDS_SERVICE_REMOVED;
      } else if (cmdLine->reinstallRequested()) {
         stringId = IDS_SERVICE_REINSTALLED;
      } else {
         _ASSERT(false);
         return;
      }

      if (!cmdLine->beSilent()) {
         auto strCaption = MainSubsystem().StringTable().getString(IDS_MBC_TVNSERVER);
         auto strText = MainSubsystem().StringTable().getString(stringId);
         MainSubsystem().message_box({}, strText, strCaption, ::user::e_message_box_ok | MB_ICONINFORMATION);
      }
   }
} // namespace remoting_node_desktop