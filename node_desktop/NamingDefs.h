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

#pragma once


// This file contains all public names.
#include "subsystem/_common_header.h"

class NamingDefs
{
public:
  static const char MIRROR_DRIVER_MESSAGE_WINDOW_CLASS_NAME[];
};

class ProductNames
{
public:
  static const char PRODUCT_NAME[];
  static const char SERVER_PRODUCT_NAME[];
};

class RegistryPaths
{
public:
  static const char SERVER_PATH[];
  static const char SERVER_REVERSE_CONN_HISTORY_PATH[];
  static const char DISPATCHER_CONN_HISTORY_PATH[];
};

class WindowNames
{
public:
  static const char WINDOW_CLASS_NAME[];
};

class LogNames
{
public:
  static const char SERVER_LOG_FILE_STUB_NAME[];
  static const char VIEWER_LOG_FILE_STUB_NAME[];
  static const char LOG_DIR_NAME[];
  static const char LOG_PIPE_PUBLIC_NAME[];
  static const char WIN_EVENT_PROVIDER_NAME[];
};

class ClipboardNames
{
public:
  static const char CLIPBOARD_WIN_CLASS_NAME[];
};

class ServiceNames
{
public:
  static const char TVNCONTROL_START_REGISTRY_ENTRY_NAME[];
  static const char SERVICE_NAME[];
  static const char SERVICE_NAME_TO_DISPLAY[];
};

class ServerApplicationNames
{
public:
  static const char SERVER_INSTANCE_MUTEX_NAME[];
  static const char CONTROL_APP_INSTANCE_MUTEX_NAME[];
  static const char FOR_SERVICE_CONTROL_APP_PIPE_NAME[];
  static const char FOR_APP_CONTROL_APP_SERVICE_PIPE_NAME[];
};

class HookDefinitions
{
public:
  static const char HOOK_LOADER_WINDOW_CLASS[];
  static const char HOOK_TARGET_WIN_CLASS_NAME[];
  static const char HOOK_LOADER_NAME[];
  static const unsigned int LOADER_CLOSE_CODE;
  static const unsigned int SPEC_IPC_CODE;
};

class DefaultNames
{
public:
  static const char DEFAULT_COMPUTER_NAME[];
};

class HttpStrings
{
public:
  static const char HTTP_INDEX_PAGE_FORMAT[];
  static const char HTTP_MSG_BADPARAMS[];
};

//// _PUBLIC_NAMES_H_
