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

#pragma once


//#include "log_writer/LogWriter.h"
#include "remoting/remoting_rtc/network/RfbInputGate.h"

#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "remoting/remoting_rtc/rfb/EncodingDefs.h"
////#include "subsystem/thread/lockable_critical_section.h"
////#include "subsystem/thread/lockable_critical_section.h"
///

namespace remoting_client
{
   class CLASS_DECL_REMOTING Decoder :
   virtual public ::particle
   {
   public:
      Decoder(::subsystem::LogWriter * plogwriter);
      virtual ~Decoder();

      //
      // This method return encoding of this Decoder.
      //
      virtual int getCode() const;

      //
      // This method return true, if decoder responsible for pseudo encoding.
      //
      virtual bool isPseudo() const;

      //
      // This static method return true, if "encoding" is pseudo encoding.
      //
      static bool isPseudo(int encoding);

   protected:
     ::pointer < ::subsystem::LogWriter > m_plogwriter;

      int m_encoding;
   };
} // namespace remoting_client