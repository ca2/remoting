// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "Decoder.h"

namespace remoting_client
{
   Decoder::Decoder(::subsystem::LogWriter * plogwriter)
   : m_plogwriter(plogwriter)
   {
   }

   Decoder::~Decoder()
   {
   }

   int Decoder::getCode() const
   {
      return m_encoding;
   }

   bool Decoder::isPseudo() const
   {
      return isPseudo(getCode());
   }

   bool Decoder::isPseudo(int encoding)
   {
      switch (encoding) {
         case ::remoting_rtc::EncodingDefs::RAW:
         case ::remoting_rtc::EncodingDefs::COPYRECT:
         case ::remoting_rtc::EncodingDefs::RRE:
         case ::remoting_rtc::EncodingDefs::HEXTILE:
         case ::remoting_rtc::EncodingDefs::TIGHT:
         case ::remoting_rtc::EncodingDefs::ZRLE:
            return false;

         case ::remoting_rtc::PseudoEncDefs::COMPR_LEVEL_0:
         case ::remoting_rtc::PseudoEncDefs::COMPR_LEVEL_1:
         case ::remoting_rtc::PseudoEncDefs::COMPR_LEVEL_2:
         case ::remoting_rtc::PseudoEncDefs::COMPR_LEVEL_3:
         case ::remoting_rtc::PseudoEncDefs::COMPR_LEVEL_4:
         case ::remoting_rtc::PseudoEncDefs::COMPR_LEVEL_5:
         case ::remoting_rtc::PseudoEncDefs::COMPR_LEVEL_6:
         case ::remoting_rtc::PseudoEncDefs::COMPR_LEVEL_7:
         case ::remoting_rtc::PseudoEncDefs::COMPR_LEVEL_8:
         case ::remoting_rtc::PseudoEncDefs::COMPR_LEVEL_9:

         case ::remoting_rtc::PseudoEncDefs::QUALITY_LEVEL_0:
         case ::remoting_rtc::PseudoEncDefs::QUALITY_LEVEL_1:
         case ::remoting_rtc::PseudoEncDefs::QUALITY_LEVEL_2:
         case ::remoting_rtc::PseudoEncDefs::QUALITY_LEVEL_3:
         case ::remoting_rtc::PseudoEncDefs::QUALITY_LEVEL_4:
         case ::remoting_rtc::PseudoEncDefs::QUALITY_LEVEL_5:
         case ::remoting_rtc::PseudoEncDefs::QUALITY_LEVEL_6:
         case ::remoting_rtc::PseudoEncDefs::QUALITY_LEVEL_7:
         case ::remoting_rtc::PseudoEncDefs::QUALITY_LEVEL_8:
         case ::remoting_rtc::PseudoEncDefs::QUALITY_LEVEL_9:

         case ::remoting_rtc::PseudoEncDefs::DESKTOP_SIZE:
         case ::remoting_rtc::PseudoEncDefs::X_CURSOR:
         case ::remoting_rtc::PseudoEncDefs::RICH_CURSOR:
         case ::remoting_rtc::PseudoEncDefs::POINTER_POS:
         case ::remoting_rtc::PseudoEncDefs::LAST_RECT:
            return true;
         default:
            // if the encoding type is unknown, then generate assertions.
            ASSERT(true);
            return true;
      }
   }
} // namespace remoting_client

