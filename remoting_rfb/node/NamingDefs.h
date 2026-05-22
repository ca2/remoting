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

#pragma once


// This file contains all public names.
#include "subsystem/_common_header.h"

namespace remoting_rfb_node
{

   class CLASS_DECL_REMOTING_RFB NamingDefs
   {
   public:
      static ::string_literal MIRROR_DRIVER_MESSAGE_WINDOW_CLASS_NAME;
   };

   class CLASS_DECL_REMOTING_RFB ProductNames
   {
   public:
      static ::string_literal PRODUCT_NAME;
      static ::string_literal SERVER_PRODUCT_NAME;
   };

   class CLASS_DECL_REMOTING_RFB RegistryPaths
   {
   public:
      static ::string_literal SERVER_PATH;
      static ::string_literal SERVER_REVERSE_CONN_HISTORY_PATH;
      static ::string_literal DISPATCHER_CONN_HISTORY_PATH;
   };

   class CLASS_DECL_REMOTING_RFB WindowNames
   {
   public:
      static ::string_literal WINDOW_CLASS_NAME;
   };

   class CLASS_DECL_REMOTING_RFB LogNames
   {
   public:
      static ::string_literal SERVER_LOG_FILE_STUB_NAME;
      static ::string_literal VIEWER_LOG_FILE_STUB_NAME;
      static ::string_literal LOG_DIR_NAME;
      static ::string_literal LOG_PIPE_PUBLIC_NAME;
      static ::string_literal WIN_EVENT_PROVIDER_NAME;
   };

    class CLASS_DECL_REMOTING_RFB ClipboardNames
   {
    public:
      static ::string_literal CLIPBOARD_WIN_CLASS_NAME;
    };

   class CLASS_DECL_REMOTING_RFB ServiceNames
   {
   public:
      static ::string_literal TVNCONTROL_START_REGISTRY_ENTRY_NAME;
      static ::string_literal SERVICE_NAME;
      static ::string_literal SERVICE_NAME_TO_DISPLAY;
   };

   class CLASS_DECL_REMOTING_RFB ServerApplicationNames
   {
   public:
      static ::string_literal SERVER_INSTANCE_MUTEX_NAME;
      static ::string_literal CONTROL_APP_INSTANCE_MUTEX_NAME;
      static ::string_literal FOR_SERVICE_CONTROL_APP_PIPE_NAME;
      static ::string_literal FOR_APP_CONTROL_APP_SERVICE_PIPE_NAME;
   };

   class CLASS_DECL_REMOTING_RFB HookDefinitions
   {
   public:
      static ::string_literal HOOK_LOADER_WINDOW_CLASS;
      static ::string_literal HOOK_TARGET_WIN_CLASS_NAME;
      static ::string_literal HOOK_LOADER_NAME;
      static const ::u32 LOADER_CLOSE_CODE;
      static const ::u32 SPEC_IPC_CODE;
   };

   class CLASS_DECL_REMOTING_RFB DefaultNames
   {
   public:
      static ::string_literal DEFAULT_COMPUTER_NAME;
   };

   class CLASS_DECL_REMOTING_RFB HttpStrings
   {
   public:
      static ::string_literal HTTP_INDEX_PAGE_FORMAT;
      static ::string_literal HTTP_MSG_BADPARAMS;
   };


} // namespace remoting_rfb_node
 


