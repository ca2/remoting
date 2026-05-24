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
#include "JpegQualityLevel.h"
#include "subsystem/platform/Exception.h"

namespace remoting_rfb_client
{
   JpegQualityLevel::JpegQualityLevel(::subsystem::LogWriter * plogwriter, int quality)
   : PseudoDecoder(plogwriter)
   {
      m_encoding = qualityToEncoding(quality);
   }

   JpegQualityLevel::~JpegQualityLevel()
   {
   }

   int JpegQualityLevel::qualityToEncoding(int qualityLevel)
   {
      switch (qualityLevel) {
         case 0: return ::remoting_rfb::PseudoEncDefs::QUALITY_LEVEL_0;
         case 1: return ::remoting_rfb::PseudoEncDefs::QUALITY_LEVEL_1;
         case 2: return ::remoting_rfb::PseudoEncDefs::QUALITY_LEVEL_2;
         case 3: return ::remoting_rfb::PseudoEncDefs::QUALITY_LEVEL_3;
         case 4: return ::remoting_rfb::PseudoEncDefs::QUALITY_LEVEL_4;
         case 5: return ::remoting_rfb::PseudoEncDefs::QUALITY_LEVEL_5;
         case 6: return ::remoting_rfb::PseudoEncDefs::QUALITY_LEVEL_6;
         case 7: return ::remoting_rfb::PseudoEncDefs::QUALITY_LEVEL_7;
         case 8: return ::remoting_rfb::PseudoEncDefs::QUALITY_LEVEL_8;
         case 9: return ::remoting_rfb::PseudoEncDefs::QUALITY_LEVEL_9;
         default:
            ::string error;
            error.format("Quality of jpeg \"{}\" is not valid", qualityLevel);
            throw ::subsystem::Exception(error);
      }
   }
} // namespace remoting_rfb_client


