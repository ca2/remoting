// #ifndef _UPDATE_REQUEST_SENDER_
// #define _UPDATE_REQUEST_SENDER_


#pragma once

#include "subsystem/thread/Thread.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"
//// #include aaa_<log_writer/LogWriter.h>
#include "remoting/remoting/network/RfbOutputGate.h"

namespace remoting_client
{


   class CLASS_DECL_REMOTING UpdateRequestSender :
      virtual public ::subsystem::Thread
   {
   public:


      UpdateRequestSender(LockableBase* plockableFramebuffer, ::innate_subsystem::Framebuffer * pframebuffer, ::subsystem::LogWriter* plogwriter);

      ~UpdateRequestSender();

      
      void destroy() override;
      
      void setWasUpdated();
      void setTimeout(int miliseconds);
      void setIsIncremental(bool isIncremental);
      void setOutput(::remoting::RfbOutputGate* output);

      class ::time getTimeout();

   //protected:
      void onThreadMain() override;


   //private:
      void sendFbUpdateRequest();

      bool isUpdated();
      bool isIncremental();
      ::remoting::RfbOutputGate* getOutput();

      bool m_bWasUpdateReceived;
      lockable_critical_section m_criticalsectionWasUpdated;

      class ::time m_timeTimeout;
      lockable_critical_section m_criticalsectionTimeout;

      bool m_bIncremental;
      lockable_critical_section m_criticalsectionIncremental;

      LockableBase *m_criticalsectionFramebuffer;
      ::pointer < ::innate_subsystem::Framebuffer > m_pframebuffer;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      ::pointer < ::remoting::RfbOutputGate > m_prfboutputgate;
      lockable_critical_section m_outputLock;
   };

   //_UPDATE_REQUEST_SENDER_
} // namespace remoting_client
