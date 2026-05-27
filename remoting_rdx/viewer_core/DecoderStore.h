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


//#include aaa_<map>
//#include aaa_<vector>

#include "Decoder.h"

//#include "log_writer/LogWriter.h"

namespace remoting_rfb_client
{
   class CLASS_DECL_REMOTING_RDX DecoderStore
   {
   public:
      DecoderStore(::subsystem::LogWriter * plogwriter);
      ~DecoderStore();

      Decoder *getDecoder(::i32 decoderId);
      ::array_base<::i32> getDecoderIds();

      // return true, if adding is complete
      // return false, if decoder already exist
      bool addDecoder(Decoder *newDecoder, ::i32 priority);
      // return true, if deleting is complete
      // return false, if decoder not exist
      bool removeDecoder(::i32 decoderId);

      void setPreferredEncoding(::i32 encodingType);
      void allowCopyRect(bool allow);

   private:
      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      ::map<::i32, ::pair<::i32, ::pointer < Decoder >> > m_decoders;
      ::i32 m_preferredEncoding;
      bool m_allowCopyRect;
   };
} // namespace remoting_rfb_client
