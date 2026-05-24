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


#include "acme/input_output/DataOutputStream.h"
#include "acme/input_output/BufferedOutputStream.h"

//#include "subsystem/thread/lockable_critical_section.h"


namespace remoting_rfb
{
   /**
    * Gate for writting rfb messages.
    *
    * @features: pblockinggate is synchonized (can be locked and unlocked, supports data buffering, and writting
    * typized data).
    * @remark: after every scopedstrMessage you want to send to must manually call flush() cause
    * "autoflush on unlock" is removed.
    * @author enikey.
    */
   class CLASS_DECL_REMOTING_RDX RfbOutputGate :
      virtual public ::DataOutputStream,
      virtual public lockable_critical_section
   {
   public:
      RfbOutputGate();
      /**
       * Creates new rfb output pblockinggate.
       * @param poutputstream real output stream.
       */
      RfbOutputGate(::OutputStream *poutputstream);

      /**
       * Deletes rfb output pblockinggate.
       */
      virtual ~RfbOutputGate();


      virtual void _initialize_rfb_output_gate(::OutputStream *poutputstream);

      /**
       * Flushes inner buffer to real output stream.
       * @throws ::io_exception on error.
       */
      virtual void flush();

   //private:
      /**
       * Tunnel that adds buffering.
       */
      ::pointer < ::BufferedOutputStream > m_pbufferedoutputstreamTunnel;
   };
} // namespace remoting_rfb
