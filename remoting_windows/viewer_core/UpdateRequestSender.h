// #ifndef _UPDATE_REQUEST_SENDER_
// #define _UPDATE_REQUEST_SENDER_


#pragma once

#include "subsystem/thread/Thread.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"
//// #include aaa_<log_writer/LogWriter.h>
#include "remoting/remoting/network/RfbOutputGate.h"

namespace remoting
{
   class CLASS_DECL_REMOTING UpdateRequestSender : public ::subsystem::Thread
   {
   public:
      UpdateRequestSender(LockableBase* plockableFramebuffer, ::innate_subsystem::Framebuffer * pframebuffer, ::subsystem::LogWriter* plogwriter);

      ~UpdateRequestSender();

      void setWasUpdated();
      void setTimeout(int miliseconds);
      void setIsIncremental(bool isIncremental);
      void setOutput(::remoting::RfbOutputGate* output);

      int getTimeout();

   //protected:
      virtual void execute() override;


   private:
      void sendFbUpdateRequest();

      bool isUpdated();
      bool isIncremental();
      ::remoting::RfbOutputGate* getOutput();

      bool m_bWasUpdateReceived;
      lockable_critical_section m_criticalsectionWasUpdated;

      int m_iTimeout;
      lockable_critical_section m_criticalsectionTimeout;

      bool m_bIncremental;
      lockable_critical_section m_criticalsectionIncremental;

      LockableBase *m_criticalsectionFramebuffer;
      ::innate_subsystem::Framebuffer *m_pframebuffer;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      ::remoting::RfbOutputGate *m_output;
      lockable_critical_section m_outputLock;
   };

   //_UPDATE_REQUEST_SENDER_
} // namespace remoting