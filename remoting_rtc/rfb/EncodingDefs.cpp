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
#include "framework.h"
#include "remoting/remoting_rfb/rfb/EncodingDefs.h"

namespace remoting_rfb
{
   const_char_pointer const EncodingDefs::SIG_RAW = "RAW_____";
   const_char_pointer const EncodingDefs::SIG_COPYRECT = "COPYRECT";
   const_char_pointer const EncodingDefs::SIG_RRE = "RRE_____";
   const_char_pointer const EncodingDefs::SIG_HEXTILE = "HEXTILE_";
   const_char_pointer const EncodingDefs::SIG_TIGHT = "TIGHT___";
   const_char_pointer const EncodingDefs::SIG_ZRLE = "ZRLE____";

   const_char_pointer const PseudoEncDefs::SIG_COMPR_LEVEL = "COMPRLVL";
   const_char_pointer const PseudoEncDefs::SIG_X_CURSOR = "X11CURSR";
   const_char_pointer const PseudoEncDefs::SIG_RICH_CURSOR = "RCHCURSR";
   const_char_pointer const PseudoEncDefs::SIG_POINTER_POS = "POINTPOS";
   const_char_pointer const PseudoEncDefs::SIG_LAST_RECT = "LASTRECT";
   const_char_pointer const PseudoEncDefs::SIG_DESKTOP_SIZE = "NEWFBSIZ";
   const_char_pointer const PseudoEncDefs::SIG_QUALITY_LEVEL = "JPEGQLVL";
   const_char_pointer const PseudoEncDefs::SIG_DESKTOP_CONFIGURATION = "NEWFBCNF";
} // namespace remoting_rfb