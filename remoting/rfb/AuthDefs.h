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

//#include "subsystem/platform/inttypes.h"

namespace remoting
{
   class CLASS_DECL_REMOTING SecurityDefs
   {
   public:
      static constexpr ::u32 INVALID = 0;
      static constexpr ::u32 NONE = 1;
      static constexpr ::u32 VNC = 2;
      static constexpr ::u32 TIGHT = 16;
      static ::u32 convertFromAuthType(::u32 authType);
   };

   class CLASS_DECL_REMOTING AuthDefs
   {
   public:
      inline static const ::u32 NONE = 1;
      inline static const ::u32 VNC = 2;
      inline static const ::u32 EXTERNAL = 130;

      static const char *const SIG_NONE;
      static const char *const SIG_VNC;
      static const char *const SIG_EXTERNAL;

      // Return TightVNC authentication method corresponding to a VNC-style
      // security type. Returns 0 if the specified security type does not ::map
      // to any valid authentication type supported in TightVNC.
      static ::u32 convertFromSecurityType(::u32 securityType);
   };

   //// __RFB_AUTH_DEFS_H_INCLUDED__
} // namespace remoting


