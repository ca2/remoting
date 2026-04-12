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


#include "subsystem/io/DataOutputStream.h"
#include "subsystem/io/BufferedOutputStream.h"

//#include "subsystem/thread/critical_section.h"


namespace remoting
{
   /**
    * Gate for writting rfb messages.
    *
    * @features: gate is synchonized (can be locked and unlocked, supports data buffering, and writting
    * typized data).
    * @remark: after every scopedstrMessage you want to send to must manually call flush() cause
    * "autoflush on unlock" is removed.
    * @author enikey.
    */
   class CLASS_DECL_REMOTING_COMMON RfbOutputGate : public ::subsystem::DataOutputStream,
                         public critical_section
   {
   public:
      /**
       * Creates new rfb output gate.
       * @param stream real output stream.
       */
      RfbOutputGate(::subsystem::OutputStream *stream);
      /**
       * Deletes rfb output gate.
       */
      virtual ~RfbOutputGate();

      /**
       * Flushes inner buffer to real output stream.
       * @throws ::io_exception on error.
       */
      virtual void flush();

   private:
      /**
       * Tunnel that adds buffering.
       */
      ::subsystem::BufferedOutputStream *m_tunnel;
   };
} // namespace remoting