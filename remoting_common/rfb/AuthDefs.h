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

#pragma once

#include "remoting/remoting_common/util/inttypes.h"

class CLASS_DECL_REMOTING_COMMON SecurityDefs
{
public:
  static const unsigned int INVALID = 0;
  static const unsigned int NONE = 1;
  static const unsigned int VNC = 2;
  static const unsigned int TIGHT = 16;
  static unsigned int convertFromAuthType(unsigned int authType);
};

class CLASS_DECL_REMOTING_COMMON AuthDefs
{
public:
  static const unsigned int NONE = 1;
  static const unsigned int VNC = 2;
  static const unsigned int EXTERNAL = 130;

  static const char *const SIG_NONE;
  static const char *const SIG_VNC;
  static const char *const SIG_EXTERNAL;

  // Return TightVNC authentication method corresponding to a VNC-style
  // security type. Returns 0 if the specified security type does not ::map
  // to any valid authentication type supported in TightVNC.
  static unsigned int convertFromSecurityType(unsigned int securityType);
};

//// __RFB_AUTH_DEFS_H_INCLUDED__
