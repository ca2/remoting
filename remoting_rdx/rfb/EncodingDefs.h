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

namespace remoting_rfb
{
   class CLASS_DECL_REMOTING_RDX EncodingDefs
   {
   public:
      static constexpr ::i32 RAW = 0;
      static constexpr ::i32 COPYRECT = 1;
      static constexpr ::i32 RRE = 2;
      static constexpr ::i32 HEXTILE = 5;
      static constexpr ::i32 TIGHT = 7;
      static constexpr ::i32 ZRLE = 16;

      static const_char_pointer const SIG_RAW;
      static const_char_pointer const SIG_COPYRECT;
      static const_char_pointer const SIG_RRE;
      static const_char_pointer const SIG_HEXTILE;
      static const_char_pointer const SIG_TIGHT;
      static const_char_pointer const SIG_ZRLE;
   };

   //
   // Codes for pseudo-encoding types
   //

   class CLASS_DECL_REMOTING_RDX PseudoEncDefs
   {
   public:
      static constexpr ::i32 COMPR_LEVEL_0 = -256;
      static constexpr ::i32 COMPR_LEVEL_1 = -255;
      static constexpr ::i32 COMPR_LEVEL_2 = -254;
      static constexpr ::i32 COMPR_LEVEL_3 = -253;
      static constexpr ::i32 COMPR_LEVEL_4 = -252;
      static constexpr ::i32 COMPR_LEVEL_5 = -251;
      static constexpr ::i32 COMPR_LEVEL_6 = -250;
      static constexpr ::i32 COMPR_LEVEL_7 = -249;
      static constexpr ::i32 COMPR_LEVEL_8 = -248;
      static constexpr ::i32 COMPR_LEVEL_9 = -247;

      static constexpr ::i32 X_CURSOR = -240;
      static constexpr ::i32 RICH_CURSOR = -239;
      static constexpr ::i32 POINTER_POS = -232;

      static constexpr ::i32 LAST_RECT = -224;
      static constexpr ::i32 DESKTOP_SIZE = -223;
      static constexpr ::i32 DESKTOP_CONFIGURATION = -222;

      static constexpr ::i32 QUALITY_LEVEL_0 = -32;
      static constexpr ::i32 QUALITY_LEVEL_1 = -31;
      static constexpr ::i32 QUALITY_LEVEL_2 = -30;
      static constexpr ::i32 QUALITY_LEVEL_3 = -29;
      static constexpr ::i32 QUALITY_LEVEL_4 = -28;
      static constexpr ::i32 QUALITY_LEVEL_5 = -27;
      static constexpr ::i32 QUALITY_LEVEL_6 = -26;
      static constexpr ::i32 QUALITY_LEVEL_7 = -25;
      static constexpr ::i32 QUALITY_LEVEL_8 = -24;
      static constexpr ::i32 QUALITY_LEVEL_9 = -23;

      static const_char_pointer const SIG_COMPR_LEVEL;
      static const_char_pointer const SIG_X_CURSOR;
      static const_char_pointer const SIG_RICH_CURSOR;
      static const_char_pointer const SIG_POINTER_POS;
      static const_char_pointer const SIG_LAST_RECT;
      static const_char_pointer const SIG_DESKTOP_SIZE;
      static const_char_pointer const SIG_QUALITY_LEVEL;
      static const_char_pointer const SIG_DESKTOP_CONFIGURATION;
   };

   //// __RFB_ENCODING_DEFS_H_INCLUDED__
} // namespace remoting_rfb
