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
#include "framework.h"
#include "ClipboardExchange.h"
#include "remoting/remoting/rfb/MsgDefs.h"
////#include "subsystem/platform/::string.h"
//#include "subsystem/platform/::string.h"
//#include "subsystem/thread/lockable_critical_section.h"
#include "remoting/remoting/rfb/VendorDefs.h"
#include "subsystem/platform/Exception.h"

namespace remoting
{
   ClipboardExchange::ClipboardExchange(RfbCodeRegistrator *m_prfbcoderegistrator,
                                        Desktop *desktop,
                                        ::remoting::RfbOutputGate *output,
                                        bool viewOnly,
                                        ::subsystem::LogWriter * plogwriter)
   : m_pdesktop(desktop),
     m_output(output),
     m_viewOnly(viewOnly),
     m_hasNewClip(false),
     m_isUtf8ClipboardEnabled(false),
     m_plogwriter(plogwriter)
   {
      // Request code
      m_prfbcoderegistrator->regCode(ClientMsgDefs::CLIENT_CUT_TEXT, this);

      m_prfbcoderegistrator->addClToSrvCap(ClientMsgDefs::CLIENT_CUT_TEXT_UTF8, VendorDefs::TIGHTVNC, Utf8CutTextDefs::CLIENT_CUT_TEXT_UTF8_SIG);
      m_prfbcoderegistrator->addClToSrvCap(ClientMsgDefs::ENABLE_CUT_TEXT_UTF8, VendorDefs::TIGHTVNC, Utf8CutTextDefs::ENABLE_CUT_TEXT_UTF8_SIG);
      m_prfbcoderegistrator->addSrvToClCap(ServerMsgDefs::SERVER_CUT_TEXT_UTF8, VendorDefs::TIGHTVNC, Utf8CutTextDefs::SERVER_CUT_TEXT_UTF8_SIG);
      m_prfbcoderegistrator->regCode(ClientMsgDefs::CLIENT_CUT_TEXT_UTF8, this);
      m_prfbcoderegistrator->regCode(ClientMsgDefs::ENABLE_CUT_TEXT_UTF8, this);

      resumeThread();
   }

   ClipboardExchange::~ClipboardExchange()
   {
//      setThreadToFinish();
//      waitThreadToFinish();
   }
void ClipboardExchange::destroy()
{
   ::subsystem::Thread::destroy();
//   setThreadToFinish();
//   waitThreadToFinish();
}

   void ClipboardExchange::onRequest(::u32 reqCode, ::remoting::RfbInputGate *prfbinputgate)
   {

      switch (reqCode) {
         case ClientMsgDefs::CLIENT_CUT_TEXT:
            prfbinputgate->readUInt8(); // pad
            prfbinputgate->readUInt16(); // pad
            onRequestWorker(false, prfbinputgate);
            break;
         case ClientMsgDefs::CLIENT_CUT_TEXT_UTF8:
            onRequestWorker(true, prfbinputgate);
            break;
         case ClientMsgDefs::ENABLE_CUT_TEXT_UTF8:
            m_isUtf8ClipboardEnabled = true;
            break;
         default:
            ::string errMess;
            errMess.format("Unknown {} protocol code received", (int)reqCode);
            throw ::subsystem::Exception(errMess);
            break;
      }
   }
   void ClipboardExchange::onRequestWorker(bool utf8flag, ::remoting::RfbInputGate *prfbinputgate)
   {
      ::u32 length = prfbinputgate->readUInt32();

      ::array_base<char> charBuff(length + 1);

      prfbinputgate->readFully(charBuff.data(), length);
      charBuff[length] = '\0';
      if (m_viewOnly) {
         return;
      }

      ::string clipText;
      if (utf8flag) {
         m_plogwriter->debug("UTF8 ClientCutText, payload length {}", length);
               clipText = charBuff.data();
//charBuff.data());
  //       utfText.toStringStorage(&clipText);
      }
      else
      {
         m_plogwriter->debug("ClientCutText, payload length {}", length);
         //::string ansiText(&charBuff.front());
         //ansiText.toStringStorage(&clipText);
         clipText = charBuff.data();
      }
      m_pdesktop->setNewClipText(clipText);
   }

   void ClipboardExchange::sendClipboard(const ::scoped_string & newClipboard)
   {
      critical_section_lock al(&m_criticalsectionStoredClip);
      m_storedClip = *newClipboard;
      m_hasNewClip = true;
      m_happeningNewClip.set_happening();
   }

   void ClipboardExchange::onTermThread()
   {
      m_happeningNewClip.set_happening();
   }

   void ClipboardExchange::onThreadMain()
   {
      while (!isThreadTerminating()) {
         m_happeningNewClip.wait();

         if (m_hasNewClip && !isThreadTerminating() && !m_viewOnly) {

            try {
               const char * data;
               size_t length;
               critical_section_lock al(m_output);
               if (m_isUtf8ClipboardEnabled) {
                  m_output->writeUInt32(ServerMsgDefs::SERVER_CUT_TEXT_UTF8); // type
                  ::string charBuff;
                  {
                     critical_section_lock al(&m_criticalsectionStoredClip);
                     charBuff = m_storedClip;
                     m_hasNewClip = false;
                  }
                  data = charBuff;
                  length = charBuff.length();
                  m_plogwriter->debug("Sending Utf8 Clipboard, payload length {}", length);
                  m_output->writeUInt32((::u32)length);
                  m_output->write(data, length);
               }
               else {
                  m_output->writeUInt8(ServerMsgDefs::SERVER_CUT_TEXT); // type
                  m_output->writeUInt8(0); // pad
                  m_output->writeUInt16(0); // pad
                  ::string charBuff;
                  {
                     critical_section_lock al(&m_criticalsectionStoredClip);
                     charBuff = m_storedClip;
                     m_hasNewClip = false;
                  }
                  data = charBuff;
                  length = charBuff.length();
                  m_plogwriter->debug("Sending Clipboard, payload length {}", length);
                  m_output->writeUInt32((::u32)length);
                  m_output->write(data, length);
               }
               m_output->flush();
            } catch (::exception &e) {
               m_plogwriter->error("The clipboard thread force to setThreadToFinish because"
                          " it caught the error: {}", e.get_message());
               //setThreadToFinish();
               setThreadToFinish();
            }
         }
      }
   }
} // namespace remoting

