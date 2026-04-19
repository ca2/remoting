// Copyright (C) 2012 GlavSoft LLC.
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
#include "NamingDefs.h"

const char NamingDefs::MIRROR_DRIVER_MESSAGE_WINDOW_CLASS_NAME[] =
  "MIRROR_DRIVER_MESSAGE_WINDOW_CLASS_NAME";

const char ProductNames::PRODUCT_NAME[] = "TightVNC";
const char ProductNames::SERVER_PRODUCT_NAME[] = "TightVNC Server";

const char RegistryPaths::SERVER_PATH[] = "Software\\TightVNC\\Server";
const char RegistryPaths::SERVER_REVERSE_CONN_HISTORY_PATH[] =
  "Software\\TightVNC\\::innate_subsystem::Control\\ReverseConnectionHistory";
const char RegistryPaths::DISPATCHER_CONN_HISTORY_PATH[] =
  "Software\\TightVNC\\::innate_subsystem::Control\\DispatcherConnectionHistory";

const char WindowNames::WINDOW_CLASS_NAME[] =
  "TvnWindowsApplicationClass";

const char LogNames::SERVER_LOG_FILE_STUB_NAME[] = "remoting_node";
const char LogNames::VIEWER_LOG_FILE_STUB_NAME[] = "remoting_impact";
const char LogNames::LOG_DIR_NAME[] = "TightVNC";
const char LogNames::LOG_PIPE_PUBLIC_NAME[] = "TVN_log_pipe_public_name";
const char LogNames::WIN_EVENT_PROVIDER_NAME[] = "remoting_node";

const char ClipboardNames::CLIPBOARD_WIN_CLASS_NAME[] = 
  "Tvnserver.Clipboard.MessageWindow";


const char ServiceNames::TVNCONTROL_START_REGISTRY_ENTRY_NAME[] =
  "tvncontrol";
const char ServiceNames::SERVICE_NAME[] = "remoting_node";
const char ServiceNames::SERVICE_NAME_TO_DISPLAY[] = "TightVNC Server";

const char ServerApplicationNames::SERVER_INSTANCE_MUTEX_NAME[] =
  "tvnserverApplication";
const char ServerApplicationNames::CONTROL_APP_INSTANCE_MUTEX_NAME[] =
  "tvnserverServiceControlSlave";
const char ServerApplicationNames::FOR_SERVICE_CONTROL_APP_PIPE_NAME[] =
  "TightVNC_Service_Control";
const char ServerApplicationNames::FOR_APP_CONTROL_APP_SERVICE_PIPE_NAME[] =
  "TightVNC_Application_Control";

const char HookDefinitions::HOOK_LOADER_WINDOW_CLASS[] =
  "TvnHookLoaderWindowClass";
const char HookDefinitions::HOOK_TARGET_WIN_CLASS_NAME[] =
  "HookTargetWinClassName";
const char HookDefinitions::HOOK_LOADER_NAME[] = "hookldr.exe";
//const unsigned int HookDefinitions::LOADER_CLOSE_CODE =
  //RegisterWindowMessage("TVN.HOOK.LOADER.CLOSE.CODE");
//const unsigned int HookDefinitions::SPEC_IPC_CODE =
  //RegisterWindowMessage("TVN.HOOK.MESSAGE.CODE");

const char DefaultNames::DEFAULT_COMPUTER_NAME[] = "TightVNC Server";


const char HttpStrings::HTTP_INDEX_PAGE_FORMAT[] =
"<HTML>\n"
"  <HEAD><TITLE>TightVNC desktop [%.256s]</TITLE></HEAD>\n"
"  <BODY>\n"
"    <APPLET ARCHIVE=\"tightvnc-jviewer.jar\""
" CODE=\"com.glavsoft.viewer.Viewer\" WIDTH=1 HEIGHT=1>\n"
"      <PARAM NAME=\"PORT\" VALUE=\"{}\">\n"
"      <PARAM NAME=\"OpenNewWindow\" VALUE=\"YES\">\n"
"%.1024s"
"    </APPLET><BR>\n"
"    <A HREF=\"http://www.tightvnc.com/\">www.TightVNC.com</A>\n"
"  </BODY>\n"
"</HTML>\n";

const char HttpStrings::HTTP_MSG_BADPARAMS[] =
"<HTML>\n"
"  <HEAD><TITLE>TightVNC desktop</TITLE></HEAD>\n"
"  <BODY>\n"
"    <H1>Bad Parameters</H1>\n"
"    The sequence of applet parameters specified within the URL is invalid.\n"
"  </BODY>\n"
"</HTML>\n";
