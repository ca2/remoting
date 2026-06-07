#pragma once

#include "remoting/remoting_rfb_macos/_common_header.h"
#include "remoting/remoting_rfb/desktop/InputBlocker.h"

#include <ApplicationServices/ApplicationServices.h>


namespace remoting_rfb_macos
{

   class CLASS_DECL_REMOTING_MACOS MacosInputBlocker :
      virtual public ::remoting_rfb::InputBlocker
   {
   public:

      MacosInputBlocker();
      ~MacosInputBlocker() override;

      void initialize_input_blocker(::subsystem::LogWriter * plogwriter) override;

      void setKeyboardBlocking(bool block) override;
      void setMouseBlocking(bool block) override;
      void setSoftKeyboardBlocking(bool block, ::u32 timeInterval) override;
      void setSoftMouseBlocking(bool block, ::u32 timeInterval) override;

      class ::time getLastInputTime() const override;
      void correctLastTime(const class ::time & time) override;
      bool isRemoteInputAllowed() override;

   private:

      static CGEventRef eventTapCallback(
         CGEventTapProxy proxy,
         CGEventType type,
         CGEventRef event,
         void * context);

      CGEventRef processEvent(CGEventType type, CGEventRef event);
      void createEventTap();
      void destroyEventTap();

      mutable lockable_critical_section m_criticalsection;
      CFMachPortRef m_eventtap;
      CFRunLoopSourceRef m_runloopsource;
      class ::time m_lastInputTime;
      ::u32 m_timeInterval;
      bool m_keyboardBlocking;
      bool m_mouseBlocking;
      bool m_softKeyboardBlocking;
      bool m_softMouseBlocking;

   };

} // namespace remoting_rfb_macos
