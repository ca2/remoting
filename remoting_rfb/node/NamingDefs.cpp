// Copyright (C) 2012 GlavSoft LLC.
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
#include "NamingDefs.h"


namespace remoting_rfb_node
{


   ::string_literal NamingDefs::MIRROR_DRIVER_MESSAGE_WINDOW_CLASS_NAME = "MIRROR_DRIVER_MESSAGE_WINDOW_CLASS_NAME";

   ::string_literal ProductNames::PRODUCT_NAME = "Remoting";
   ::string_literal ProductNames::SERVER_PRODUCT_NAME = "Remoting Node";

   ::string_literal RegistryPaths::SERVER_PATH = "Software\\Remoting\\Server";
   ::string_literal RegistryPaths::SERVER_REVERSE_CONN_HISTORY_PATH =
      "Software\\Remoting\\::innate_subsystem::Control\\ReverseConnectionHistory";
   ::string_literal RegistryPaths::DISPATCHER_CONN_HISTORY_PATH =
      "Software\\Remoting\\::innate_subsystem::Control\\DispatcherConnectionHistory";

   ::string_literal WindowNames::WINDOW_CLASS_NAME = "TvnWindowsApplicationClass";

   ::string_literal LogNames::SERVER_LOG_FILE_STUB_NAME = "remoting_rfb_node";
   ::string_literal LogNames::VIEWER_LOG_FILE_STUB_NAME = "remoting_impact";
   ::string_literal LogNames::LOG_DIR_NAME = "Remoting";
   ::string_literal LogNames::LOG_PIPE_PUBLIC_NAME = "TVN_log_pipe_public_name";
   ::string_literal LogNames::WIN_EVENT_PROVIDER_NAME = "remoting_rfb_node";

   //::string_literal ClipboardNames::CLIPBOARD_WIN_CLASS_NAME =
   //  "Remotingnode.Clipboard.MessageWindow";


   ::string_literal ServiceNames::TVNCONTROL_START_REGISTRY_ENTRY_NAME = "tvncontrol";
   ::string_literal ServiceNames::SERVICE_NAME = "remoting_rfb_node";
   ::string_literal ServiceNames::SERVICE_NAME_TO_DISPLAY = "Remoting Node";

   ::string_literal ServerApplicationNames::SERVER_INSTANCE_MUTEX_NAME = "remotingnodeApplication";
   ::string_literal ServerApplicationNames::CONTROL_APP_INSTANCE_MUTEX_NAME = "remotingnodeServiceControlSlave";
   ::string_literal ServerApplicationNames::FOR_SERVICE_CONTROL_APP_PIPE_NAME = "Remoting_Service_Control";
   ::string_literal ServerApplicationNames::FOR_APP_CONTROL_APP_SERVICE_PIPE_NAME = "Remoting_Application_Control";

   ::string_literal HookDefinitions::HOOK_LOADER_WINDOW_CLASS = "TvnHookLoaderWindowClass";
   //::string_literal HookDefinitions::HOOK_TARGET_WIN_CLASS_NAME = "HookTargetWinClassName";
   ::string_literal HookDefinitions::HOOK_LOADER_NAME = "hookldr.exe";
   // const ::u32 HookDefinitions::LOADER_CLOSE_CODE =
   // RegisterWindowMessage("TVN.HOOK.LOADER.CLOSE.CODE");
   // const ::u32 HookDefinitions::SPEC_IPC_CODE =
   // RegisterWindowMessage("TVN.HOOK.MESSAGE.CODE");

   ::string_literal DefaultNames::DEFAULT_COMPUTER_NAME = "Remoting Node";


   ::string_literal HttpStrings::HTTP_INDEX_PAGE_FORMAT =
      "<HTML>\n"
      "  <HEAD><TITLE>Remoting desktop [%.256s]</TITLE></HEAD>\n"
      "  <BODY>\n"
      "    <APPLET ARCHIVE=\"remoting-jviewer.jar\""
      " CODE=\"com.glavsoft.viewer.Viewer\" WIDTH=1 HEIGHT=1>\n"
      "      <PARAM NAME=\"PORT\" VALUE=\"{}\">\n"
      "      <PARAM NAME=\"OpenNewWindow\" VALUE=\"YES\">\n"
      "%.1024s"
      "    </APPLET><BR>\n"
      "    <A HREF=\"http://www.t i g h t v n c.com/\">www.Remoting.com</A>\n"
      "  </BODY>\n"
      "</HTML>\n";

   ::string_literal HttpStrings::HTTP_MSG_BADPARAMS =
      "<HTML>\n"
      "  <HEAD><TITLE>Remoting desktop</TITLE></HEAD>\n"
      "  <BODY>\n"
      "    <H1>Bad Parameters</H1>\n"
      "    The sequence of applet parameters specified within the URL is invalid.\n"
      "  </BODY>\n"
      "</HTML>\n";


} // namespace remoting_rfb_node
