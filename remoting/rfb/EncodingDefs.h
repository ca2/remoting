// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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

//
// Codes for encoding types
//

namespace remoting
{
   class CLASS_DECL_REMOTING EncodingDefs
   {
   public:
      static constexpr int RAW = 0;
      static constexpr int COPYRECT = 1;
      static constexpr int RRE = 2;
      static constexpr int HEXTILE = 5;
      static constexpr int TIGHT = 7;
      static constexpr int ZRLE = 16;

      static const char *const SIG_RAW;
      static const char *const SIG_COPYRECT;
      static const char *const SIG_RRE;
      static const char *const SIG_HEXTILE;
      static const char *const SIG_TIGHT;
      static const char *const SIG_ZRLE;
   };

   //
   // Codes for pseudo-encoding types
   //

   class CLASS_DECL_REMOTING PseudoEncDefs
   {
   public:
      static constexpr int COMPR_LEVEL_0 = -256;
      static constexpr int COMPR_LEVEL_1 = -255;
      static constexpr int COMPR_LEVEL_2 = -254;
      static constexpr int COMPR_LEVEL_3 = -253;
      static constexpr int COMPR_LEVEL_4 = -252;
      static constexpr int COMPR_LEVEL_5 = -251;
      static constexpr int COMPR_LEVEL_6 = -250;
      static constexpr int COMPR_LEVEL_7 = -249;
      static constexpr int COMPR_LEVEL_8 = -248;
      static constexpr int COMPR_LEVEL_9 = -247;

      static constexpr int X_CURSOR = -240;
      static constexpr int RICH_CURSOR = -239;
      static constexpr int POINTER_POS = -232;

      static constexpr int LAST_RECT = -224;
      static constexpr int DESKTOP_SIZE = -223;
      static constexpr int DESKTOP_CONFIGURATION = -222;

      static constexpr int QUALITY_LEVEL_0 = -32;
      static constexpr int QUALITY_LEVEL_1 = -31;
      static constexpr int QUALITY_LEVEL_2 = -30;
      static constexpr int QUALITY_LEVEL_3 = -29;
      static constexpr int QUALITY_LEVEL_4 = -28;
      static constexpr int QUALITY_LEVEL_5 = -27;
      static constexpr int QUALITY_LEVEL_6 = -26;
      static constexpr int QUALITY_LEVEL_7 = -25;
      static constexpr int QUALITY_LEVEL_8 = -24;
      static constexpr int QUALITY_LEVEL_9 = -23;

      static const char *const SIG_COMPR_LEVEL;
      static const char *const SIG_X_CURSOR;
      static const char *const SIG_RICH_CURSOR;
      static const char *const SIG_POINTER_POS;
      static const char *const SIG_LAST_RECT;
      static const char *const SIG_DESKTOP_SIZE;
      static const char *const SIG_QUALITY_LEVEL;
      static const char* const SIG_DESKTOP_CONFIGURATION;
   };

   //// __RFB_ENCODING_DEFS_H_INCLUDED__
} // namespace remoting
