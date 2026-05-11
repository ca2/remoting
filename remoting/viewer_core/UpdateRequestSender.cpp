#include "framework.h"
#include "UpdateRequestSender.h"
// #include aaa_<thread/critical_section_lock.h>
#include "RfbFramebufferUpdateRequestClientMessage.h"

namespace remoting_client
{
   UpdateRequestSender::UpdateRequestSender(LockableBase* plockable, ::innate_subsystem::Framebuffer* pframebuffer, ::subsystem::LogWriter* plogwriter):
      m_bWasUpdateReceived(false),
      m_iTimeout(0),
      m_bIncremental(true),
       m_criticalsectionFramebuffer(plockable),
       m_pframebuffer(pframebuffer),
       m_plogwriter(plogwriter),
       m_prfboutputgate(0)
   {

   }

   UpdateRequestSender::~UpdateRequestSender()
   {
      try
      {
         terminate();
         wait();
      }
      catch (...)
      {
      }
   }

   void UpdateRequestSender::setWasUpdated()
   {
      critical_section_lock al(&m_criticalsectionWasUpdated);
      m_bWasUpdateReceived = true;
   }

   void UpdateRequestSender::setTimeout(int miliseconds)
   {
      critical_section_lock al(&m_criticalsectionTimeout);
      m_iTimeout = miliseconds;
   }

   void UpdateRequestSender::setIsIncremental(bool isIncremental)
   {
      critical_section_lock al(&m_criticalsectionIncremental);
      m_bIncremental = isIncremental;
   }

   void UpdateRequestSender::setOutput(::remoting::RfbOutputGate* output)
   {
      {
         critical_section_lock al(&m_outputLock);
         m_prfboutputgate = output;
      }

      // Start thread.
      resume();
      m_plogwriter->debug("UpdateRequestServer is started");
   }

   void UpdateRequestSender::execute()
   {
      try
      {
         while(!isTerminating())
         {
            if (isUpdated())
            {
               sendFbUpdateRequest();
            }

            int timeout = getTimeout();
            timeout = timeout > 0 ? timeout : 500;
            //todo: check if it is best approoach?
            Thread::sleep(timeout);
         }
      }
      catch(const ::exception &ex)
      {
         m_plogwriter->debug("UpdateRequestSender. ::subsystem::Exception: {}", ex.get_message());
      }
      catch(...)
      {
         m_plogwriter->error("UpdateRequestSender. Unknow error has occured.");
      }
   }

   void UpdateRequestSender::sendFbUpdateRequest()
   {
      ::remoting::RfbOutputGate* output = getOutput();

      if(output == 0)
         return;

      ::i32_rectangle updateRect;
      {
         AutoLock al(m_criticalsectionFramebuffer);
         updateRect = m_pframebuffer->getDimension();
      }

      bool isIncremental = this->isIncremental();

      if (isIncremental)
      {
         m_plogwriter->debug("Sending frame buffer incremental update request [%dx{}]...",
                         updateRect.width(), updateRect.height());
      }
      else
      {
         m_plogwriter->debug("Sending frame buffer full update request [%dx{}]...",
                         updateRect.width(), updateRect.height());
      }

      RfbFramebufferUpdateRequestClientMessage fbUpdReq(isIncremental, updateRect);
      fbUpdReq.send(m_prfboutputgate);
      m_plogwriter->debug("Frame buffer update request is sent");
   }

   bool UpdateRequestSender::isUpdated()
   {
      critical_section_lock al(&m_criticalsectionWasUpdated);
      bool result = m_bWasUpdateReceived;
      m_bWasUpdateReceived = false;
      return result;
   }

   int UpdateRequestSender::getTimeout()
   {
      critical_section_lock al(&m_criticalsectionTimeout);
      return m_iTimeout;
   }

   bool UpdateRequestSender::isIncremental()
   {
      critical_section_lock al(&m_criticalsectionIncremental);
      return m_bIncremental;
   }

   ::remoting::RfbOutputGate* UpdateRequestSender::getOutput()
   {
      critical_section_lock al(&m_outputLock);
      return m_prfboutputgate;
   }
} // namespace remoting_client