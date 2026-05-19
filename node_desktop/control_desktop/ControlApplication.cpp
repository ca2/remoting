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
#include "ControlApplication.h"
#include "ControlTrayIcon.h"
#include "ControlTrayIcon.h"
#include "ControlPipeName.h"
#include "remoting/remoting/node/ControlCommand.h"
#include "ReloadConfigCommand.h"
#include "DisconnectAllCommand.h"
#include "SharePrimaryCommand.h"
#include "ShareDisplayCommand.h"
#include "ShareWindowCommand.h"
#include "ShareRectCommand.h"
#include "ShareFullCommand.h"
#include "ShareAppCommand.h"
#include "ControlAuth.h"
#include "ConnectCommand.h"
#include "ShutdownCommand.h"

#include "subsystem/platform/VncPassCrypt.h"
//#include "subsystem/platform/winhdr.h"
//#include "acme/_operating_system.h"

#include "subsystem/platform/StringTable.h"
#include "remoting/remoting/node/NamingDefs.h"

#include "remoting/node_desktop/control_desktop/ControlCommandLine.h"
#include "remoting/node_desktop/ServerHelp.h"

#include "subsystem/node/OperatingSystem.h"
#include "subsystem/node/Shell.h"
#include "subsystem/node/Process.h"
#include "subsystem/platform/CommandLineArguments.h"

#include "subsystem/thread/ZombieKiller.h"
#include "subsystem/thread/GlobalMutex.h"

#include "innate_subsystem/gui/CommonControlsEx.h"
#include "innate_subsystem/platform/subsystem.h"
#include "subsystem/thread/ZombieKiller.h"
//#include "remoting/remoting/network/socket/WindowsSocket.h"

#include "remoting/node_desktop/resource.h"

#include "remoting/remoting/node_user_config/ConfigDialog.h"
//#include "subsystem/platform/::string.h"
#include "remoting/remoting/node/NamingDefs.h"
#include "SetPasswordsDialog.h"
#include "acme/platform/node.h"
#include "subsystem/node/SystemException.h"
#include "subsystem/socket/Sockets.h"
//#include aaa_<algorithm>

namespace remoting_control_desktop
{


   ControlApplication::ControlApplication(
#ifdef WINDOWS
                                          ::hinstance hinst,
                                          const ::scoped_string & scopedstrwindowClassName,
#endif
                                          const ::scoped_string & scopedstrCommandLine)
    : //WindowsApplication(hinst, windowClassName),
      //m_pcontrolproxy(0),
      m_pcontrolgate(0),
      m_ptransport(0),
      m_pcontroltrayicon(0),
      m_slaveModeEnabled(false),
m_commandLine(scopedstrCommandLine)
   //,
      //m_configurator(false)//,
      //m_plogwriter(0)
   {
      construct_newø(m_pconfigurator);
      m_pconfigurator->initialize_configurator(false);

      initialize_operating_system_application();;
      //m_commandLine= scopedstrCommandLine;


      InnateSubsystem().initializeInnateSubsystemControls();

      ///CommonControlsEx::init();
      ///
      ///

      //WindowsSocket::startup(2, 1);

      MainSubsystem().Sockets().startSockets();

   }


   ControlApplication::~ControlApplication()
   {
      // try {
      //    WindowsSocket::cleanup();
      // } catch (...) { }
      //
      //if (m_pcontrolproxy != 0) {
         //delete m_pcontrolproxy;
      //}
      //if (m_pcontrolgate != 0) {
        // delete m_pcontrolgate;
      //}
      //if (m_ptransport != 0) {
        // delete m_ptransport;
      //}
   }

   void ControlApplication::onThreadMain()
   {
      ControlCommandLine cmdLineParser;

      // Check command line for valid.
      try {
         //WinCommandLineArgs cmdArgs(m_commandLine);
         auto pcommandlinearguments = MainSubsystem().getCommandLineArguments(m_commandLine);
         ///cmdLineParser.parse(&cmdArgs);
         cmdLineParser.parse(pcommandlinearguments);
      } catch (CommandLineFormatException &) {
         ::remoting_node_desktop::ServerHelp::showUsage();
         setExitCode(1);
         return;
      }

      // Run configuration dialog and exit.
      if (cmdLineParser.hasConfigAppFlag() || cmdLineParser.hasConfigServiceFlag()) {
         int iExitCode =  runConfigurator(cmdLineParser.hasConfigServiceFlag(), cmdLineParser.hasDontElevateFlag());
         setExitCode(iExitCode);
         return;
      }

      if (cmdLineParser.hasCheckServicePasswords()) {
         int iExitCode = checkServicePasswords(cmdLineParser.hasDontElevateFlag());
         setExitCode(iExitCode);
         return;
      }

      // Change passwords and exit.
      if (cmdLineParser.hasSetControlPasswordFlag() || cmdLineParser.hasSetVncPasswordFlag()) {
         m_pconfigurator->setServiceFlag(true);
         m_pconfigurator->load();
         ::remoting_node::ServerConfig * pserverconfig = m_pconfigurator->getServerConfig();
         unsigned char cryptedPass[8];
         if (cmdLineParser.hasSetControlPasswordFlag()) {
            getCryptedPassword(cryptedPass, cmdLineParser.getControlPassword());
            pserverconfig->setControlPassword((const unsigned char *)cryptedPass);
            pserverconfig->useControlAuth(true);
         } else {
            getCryptedPassword(cryptedPass, cmdLineParser.getPrimaryVncPassword());
            pserverconfig->setPrimaryPassword((const unsigned char *)cryptedPass);
            pserverconfig->useAuthentication(true);
         }
         m_pconfigurator->save();
         setExitCode(0);
         return;
      }

      int retCode = 0;

      // If we are in the "-controlservice -slave" mode, make sure there are no
      // other "service slaves" in this session, exit if there is one already.

      ::pointer < ::mutex > appGlobalMutex;

      if (cmdLineParser.hasControlServiceFlag() && cmdLineParser.isSlave()) {
         try {
            appGlobalMutex = node()->create_local_named_mutex(this, false, ::remoting_node::ServerApplicationNames::CONTROL_APP_INSTANCE_MUTEX_NAME);
         } catch (...) {
            setExitCode(1);
            return;
         }
      }

      auto pzombiekiller = createø<::subsystem::ZombieKiller>();

      // Connect to server.
      try {
         connect(cmdLineParser.hasControlServiceFlag(), cmdLineParser.isSlave());
      } catch (::subsystem::Exception &) {
         if (!cmdLineParser.isSlave() && !cmdLineParser.hasCheckServicePasswords()) {
            const ::scoped_string & scopedstrMsg = MainSubsystem().StringTable().getString(IDS_FAILED_TO_CONNECT_TO_CONTROL_SERVER);
            const ::scoped_string & scopedstrCaption = MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL);
            MainSubsystem().message_box({}, scopedstrMsg, scopedstrCaption, ::user::e_message_box_ok | ::user::e_message_box_icon_error);
         }
         setExitCode(1);
         return;
      }

      // Execute command (if specified) and exit.
      if (cmdLineParser.isCommandSpecified()) {
         ::pointer < ::subsystem::Command > pcommand;

         ::string passwordFile;
         cmdLineParser.getPasswordFile(passwordFile);
         m_pcontrolproxy->setPasswordProperties(passwordFile, true,
                                                cmdLineParser.hasControlServiceFlag());

         if (cmdLineParser.hasKillAllFlag()) {
            pcommand = allocateø DisconnectAllCommand(m_pcontrolproxy);
         } else if (cmdLineParser.hasReloadFlag()) {
            pcommand = allocateø ReloadConfigCommand(m_pcontrolproxy);
         } else if (cmdLineParser.hasConnectFlag()) {
            ::string hostName;
            cmdLineParser.getConnectHostName(hostName);
            pcommand = allocateø ConnectCommand(m_pcontrolproxy, hostName);
         } else if (cmdLineParser.hasShutdownFlag()) {
            pcommand = allocateø ShutdownCommand(m_pcontrolproxy);
         } else if (cmdLineParser.hasSharePrimaryFlag()) {
            pcommand = allocateø SharePrimaryCommand(m_pcontrolproxy);
         } else if (cmdLineParser.hasShareDisplay()) {
            unsigned char displayNumber = cmdLineParser.getShareDisplayNumber();
            pcommand = allocateø ShareDisplayCommand(m_pcontrolproxy, displayNumber);
         } else if (cmdLineParser.hasShareWindow()) {
            ::string shareWindowName;
            cmdLineParser.getShareWindowName(shareWindowName);
            pcommand = allocateø ShareWindowCommand(m_pcontrolproxy, shareWindowName);
         } else if (cmdLineParser.hasShareRect()) {
            ::i32_rectangle shareRect = cmdLineParser.getShareRect();
            pcommand = allocateø ShareRectCommand(m_pcontrolproxy, shareRect);
         } else if (cmdLineParser.hasShareFull()) {
            pcommand = allocateø ShareFullCommand(m_pcontrolproxy);
         } else if (cmdLineParser.hasShareApp()) {
            pcommand = allocateø ShareAppCommand(m_pcontrolproxy, cmdLineParser.getSharedAppProcessId());
         }

         retCode = runControlCommand(pcommand);

         if (pcommand) {
            //delete command;
         }
      } else {
         bool showIcon = true;

         if (cmdLineParser.isSlave()) {
            m_slaveModeEnabled = true;
            try {
               try {
                  showIcon = m_pcontrolproxy->getShowTrayIconFlag();
               } catch (::remoting_node::RemoteException &remEx) {
                  notifyServerSideException(remEx.get_message());
               }
               try {
                  m_pcontrolproxy->updateTvnControlProcessId(node()->current_process_identifier());
               } catch (::remoting_node::RemoteException &remEx) {
                  notifyServerSideException(remEx.get_message());
               }
            } catch (::io_exception &) {
               notifyConnectionLost();
               setExitCode(1);
               return;
            } catch (::subsystem::Exception &) {
               ASSERT(false);
            }
         }

         retCode = runControlInterface(showIcon);
      }

      if (appGlobalMutex != 0) {
         delete appGlobalMutex;
      }

      setExitCode(retCode);
return;
      //return retCode;
   }

   void ControlApplication::connect(bool controlService, bool slave)
   {

      if (m_ptransport)
      {

         information("Trying to understand this ControlApplication... should a connect be asked if there is an existing transport?");

      }
      // Determine the name of pipe to connect to.
      ::string pipeName;
      ControlPipeName::createPipeName(controlService, pipeName, &m_plogwriter);

      int numTriesRemaining = slave ? 10 : 1;
      int msDelayBetweenTries = 2000;

      ::pointer<Transport> ptransportNew;

      while (numTriesRemaining-- > 0) {
         try {
            ptransportNew = TransportFactory::createPipeClientTransport(pipeName);
            break;
         } catch (::subsystem::Exception &) {
            if (numTriesRemaining <= 0) {
               throw;
            }
         }
         preempt(msDelayBetweenTries * 1_ms);
      }


      m_ptransport = ptransportNew;
      // We can get here only on successful connection.
      m_pcontrolgate = allocateø ControlGate(ptransportNew->getIOStream());
      m_pcontrolproxy = allocateø ControlProxy(m_pcontrolgate);
   }

   void ControlApplication::notifyServerSideException(const ::scoped_string & scopedstrReason)
   {
      ::string strMessage;

      strMessage.runtime_format(MainSubsystem().StringTable().getString(IDS_CONTROL_SERVER_RAISE_EXCEPTION), scopedstrReason);

      MainSubsystem().message_box({}, strMessage, MainSubsystem().StringTable().getString(IDS_MBC_TVNSERVER), ::user::e_message_box_ok | ::user::e_message_box_icon_error);
   }

   void ControlApplication::notifyConnectionLost()
   {
      MainSubsystem().message_box({},
                 MainSubsystem().StringTable().getString(IDS_CONTROL_CONNECTION_LOST),
                 MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
                 ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);
   }

   void ControlApplication::onOperatingSystemApplicationMain()
   {
      try {
         while (!isThreadTerminating()) {
            Thread::sleep(500);
            // If we need to show or hide icon.
            bool showIcon = m_pcontrolproxy->getShowTrayIconFlag() || !m_slaveModeEnabled;

            // Check if we need to show icon.
            if (showIcon && !m_pcontroltrayicon->isVisible()) {
               m_pcontroltrayicon->show();
            }
            // Check if we need to hide icon.
            if (m_pcontroltrayicon != 0 && !showIcon) {
               m_pcontroltrayicon->hide();
            }
            // Update tray icon status if icon is set.
            if (m_pcontroltrayicon->isVisible()) {
               m_pcontroltrayicon->syncStatusWithServer();
            }
         }
      } catch (...) {
         m_pcontroltrayicon->setThreadToFinish();
         m_pcontroltrayicon->waitForTermination();
         shutdown();
      }
   }

   int ControlApplication::runControlInterface(bool showIcon)
   {
      m_pcontroltrayicon = new ControlTrayIcon(m_pconfigurator,  m_pcontrolproxy, this, this, showIcon);

      resumeThread();

      onOperatingSystemApplicationMain();

      //setThreadToFinish();
      //waitThreadToFinish();

      delete m_pcontroltrayicon;

      return getExitCode();
   }

   int ControlApplication::runControlCommand(::subsystem::Command *command)
   {
      ::remoting_node::ControlCommand ctrlCmd(command);

      ctrlCmd.onRunCommand();

      int errorCode = ctrlCmd.executionResultOk() ? 0 : 1;
      return errorCode;
   }

   int ControlApplication::runConfigurator(bool configService, bool isRunAsRequested)
   {
      // If not enough rights to configurate service, then restart application requesting
      // admin access rights.
      if (configService && (MainSubsystem().OperatingSystem().isUserAnAdmin() == false)) {
         // If admin rights already requested and application still don't have them,
         // then show error scopedstrMessage and exit.
         if (isRunAsRequested) {
            MainSubsystem().message_box({},
              MainSubsystem().StringTable().getString(IDS_ADMIN_RIGHTS_NEEDED),
              MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
              ::user::e_message_box_ok | ::user::e_message_box_icon_error);
            return 0;
         }
         // Path to remoting_node binary.
         ::string pathToBinary;
         // Command line for child process.
         ::string childCommandLine;

         // Get path to remoting_node binary.
         pathToBinary = MainSubsystem().OperatingSystem().getCurrentModulePath();;
         // Set -dontelevate flag to tvncontrol know that admin rights already requested.
         childCommandLine.format("{} -dontelevate", m_commandLine);

         // Start child.
         try {
            MainSubsystem().Shell().runAsAdmin(pathToBinary, childCommandLine);
         } catch (::subsystem::SystemException &sysEx) {
            if (sysEx.getErrorCode().as_status() != error_cancelled) { // ERROR_CANCELLED
               MainSubsystem().message_box({},
                 sysEx.get_message(),
                 MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
                 ::user::e_message_box_ok | ::user::e_message_box_icon_error);
            }
            return 1;
         } // try / catch.
         return 0;
      }

      ::remoting_node::Configurator *configurator = m_pconfigurator;

      configurator->setServiceFlag(configService);
      configurator->load();

      ::remoting_node::ConfigDialog confDialog(configurator, nullptr);

      return confDialog.showModal();
   }

   void ControlApplication::getCryptedPassword(unsigned char cryptedPass[8], const ::scoped_string & scopedstrPlainTextPassString)
   {
      // Get a copy of the password truncated at 8 characters.
      ::string plainTextPass(scopedstrPlainTextPassString);
      //plainTextPass.getSubstring(&plainTextPass, 0, 7);
      plainTextPass.truncate(8);
      // Convert from TCHAR[] to char[].
      // FIXME: Check exception catching.
      ::string ansiPass(&plainTextPass);

      // Convert to a byte array.
      unsigned char byteArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};
      size_t len = ::minimum(ansiPass.length(), (size_t)8);
      memcpy(byteArray, ansiPass, len);

      // Encrypt with a fixed key.
      ::subsystem::VncPassCrypt::getEncryptedPass(cryptedPass, byteArray);
   }

   int ControlApplication::checkServicePasswords(bool isRunAsRequested)
   {
      // FIXME: code duplication.
      if (MainSubsystem().OperatingSystem().isUserAnAdmin() == false) {
         // If admin rights already requested and application still don't have them,
         // then show error scopedstrMessage and exit.
         if (isRunAsRequested) {
            MainSubsystem().message_box({},
              MainSubsystem().StringTable().getString(IDS_ADMIN_RIGHTS_NEEDED),
              MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
              ::user::e_message_box_ok | ::user::e_message_box_icon_error);
            return 1;
         }
         // Path to remoting_node binary.
         ::string pathToBinary;
         // Command line for child process.
         ::string childCommandLine;

         // Get path to remoting_node binary.
         pathToBinary = MainSubsystem().OperatingSystem().getCurrentModulePath();;
         // Set -dontelevate flag to tvncontrol know that admin rights already requested.
         childCommandLine.format("{} -dontelevate", m_commandLine);

         // Start child.
         try {
            MainSubsystem().Shell().runAsAdmin(pathToBinary, childCommandLine);
            return 0;
         } catch (::subsystem::SystemException &sysEx) {
            if (sysEx.getErrorCode().as_status() != error_cancelled) {
               MainSubsystem().message_box({},
                 sysEx.get_message(),
                 MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
                 ::user::e_message_box_ok | ::user::e_message_box_icon_error);
            }
            return 1;
         } // try / catch.
         return 0;
      }
      checkServicePasswords();
      return 0;
   }

   void ControlApplication::checkServicePasswords()
   {
      m_pconfigurator->setServiceFlag(true);
      m_pconfigurator->load();
      ::remoting_node::ServerConfig * pserverconfig = m_pconfigurator->getServerConfig();

      bool askToChangeRfbAuth = !pserverconfig->isUsingAuthentication() || !pserverconfig->hasPrimaryPassword();
      bool askToChangeAdmAuth = false;
      SetPasswordsDialog dialog(askToChangeRfbAuth, askToChangeAdmAuth);
      if (dialog.showModal() == ::innate_subsystem::e_control_id_ok) {
         unsigned char cryptedPass[8];
         bool useRfbAuth = dialog.getUseRfbPass();
         bool dontUseRfbAuth = dialog.getRfbPassForClear();
         // Note: The state !useRfbAuth && !dontUseRfbAuth is valid and means "do not change
         // the auth settings".
         if (useRfbAuth) {
            ::string pass;
            dialog.getRfbPass(pass);
            getCryptedPassword(cryptedPass, pass);
            pserverconfig->setPrimaryPassword(cryptedPass);
            pserverconfig->useAuthentication(true);
         } else if (dontUseRfbAuth) {
            pserverconfig->deletePrimaryPassword();
            pserverconfig->deleteReadOnlyPassword();
            pserverconfig->useAuthentication(false);
         }
         bool useAdmAuth = dialog.getUseAdmPass();
         bool dontUseAdmAuth = dialog.getAdmPassForClear();
         if (useAdmAuth) {
            ::string pass;
            dialog.getAdmPass(pass);
            getCryptedPassword(cryptedPass, pass);
            pserverconfig->setControlPassword(cryptedPass);
            pserverconfig->useControlAuth(true);
         } else if (dontUseAdmAuth) {
            pserverconfig->deleteControlPassword();
            pserverconfig->useControlAuth(false);
         }
         m_pconfigurator->save();
         reloadConfig();
      }
   }

   void ControlApplication::reloadConfig()
   {
      ::string pathToBinary;
      try {
         // Get path to remoting_node binary.
         pathToBinary = MainSubsystem().OperatingSystem().getCurrentModulePath();;
         ::subsystem::Process processToReloadConfig;
         processToReloadConfig.initialize_process(pathToBinary, "-controlservice -reload");
         processToReloadConfig.start();
      } catch (::exception &e) {
         ::string errMess;
         errMess.runtime_format(MainSubsystem().StringTable().getString(IDS_FAILED_TO_RELOAD_SERVICE_ON_CHECK_PASS), e.get_message());
         MainSubsystem().message_box({},
           errMess,
           MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
           ::user::e_message_box_ok | ::user::e_message_box_icon_error);
      }
   }
} // namespace remoting_control_desktop

