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


#include "RfbDispatcherListener.h"
#include "RfbCodeRegistrator.h"
#include "remoting/remoting/desktop/Desktop.h"
#include "remoting/remoting/network/RfbOutputGate.h"
//#include "log_writer/LogWriter.h"
#include "acme/parallelization/happening.h"

namespace remoting
{
   class ClipboardExchange : public RfbDispatcherListener, public ::subsystem::Thread
   {
   public:
      ClipboardExchange(RfbCodeRegistrator *m_prfbcoderegistrator, Desktop *desktop,
                        ::remoting::RfbOutputGate *output, bool viewOnly, ::subsystem::LogWriter * plogwriter);
      virtual ~ClipboardExchange();

      void sendClipboard(const ::scoped_string & newClipboard);

   protected:
      // Listen function
      virtual void onRequest(::u32 reqCode, ::remoting::RfbInputGate *input);
      virtual void execute();
      virtual void onTerminate();

   private:
      void onRequestWorker(bool utf8data, ::remoting::RfbInputGate *input);

      bool m_viewOnly;
      bool m_isUtf8ClipboardEnabled;
      ::pointer < Desktop > m_pdesktop;
      ::remoting::RfbOutputGate *m_output;

      //::happening m_happeningNewClip;

      ::happening m_happeningNewClip;


      ::string m_storedClip;
      bool m_hasNewClip;
      lockable_critical_section m_criticalsectionStoredClip;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting



