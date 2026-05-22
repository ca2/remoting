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


#include "RfbDispatcherListener.h"
#include "RfbCodeRegistrator.h"
#include "remoting/remoting_rfb/desktop/Desktop.h"
#include "remoting/remoting_rfb/network/RfbOutputGate.h"
//#include "log_writer/LogWriter.h"
#include "acme/parallelization/happening.h"

namespace remoting_rfb
{
   class ClipboardExchange : public RfbDispatcherListener, public ::subsystem::Thread
   {
   public:
      ClipboardExchange(RfbCodeRegistrator *m_prfbcoderegistrator, Desktop *desktop,
                        ::remoting_rfb::RfbOutputGate *output, bool viewOnly, ::subsystem::LogWriter * plogwriter);
      ~ClipboardExchange() override;
      
      
      void destroy() override;

      void sendClipboard(const ::scoped_string & newClipboard);

   protected:
      // Listen function
      void onRequest(::u32 reqCode, ::remoting_rfb::RfbInputGate *input) override;
      void onThreadMain() override;
      void onTermThread() override;

   private:
      void onRequestWorker(bool utf8data, ::remoting_rfb::RfbInputGate *input);

      bool m_viewOnly;
      bool m_isUtf8ClipboardEnabled;
      ::pointer < Desktop > m_pdesktop;
      ::remoting_rfb::RfbOutputGate *m_output;

      //::happening m_happeningNewClip;

      ::happening m_happeningNewClip;


      ::string m_storedClip;
      bool m_hasNewClip;
      lockable_critical_section m_criticalsectionStoredClip;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting_rfb



