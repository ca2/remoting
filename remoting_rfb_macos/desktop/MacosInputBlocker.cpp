#include "framework.h"
#include "remoting/remoting_rfb_macos/desktop/MacosInputBlocker.h"

#include <unistd.h>


namespace remoting_rfb_macos
{

   namespace
   {

      bool is_keyboard_event(CGEventType type)
      {
         return type == kCGEventKeyDown
            || type == kCGEventKeyUp
            || type == kCGEventFlagsChanged;
      }


      bool is_mouse_event(CGEventType type)
      {
         return type == kCGEventLeftMouseDown
            || type == kCGEventLeftMouseUp
            || type == kCGEventRightMouseDown
            || type == kCGEventRightMouseUp
            || type == kCGEventMouseMoved
            || type == kCGEventLeftMouseDragged
            || type == kCGEventRightMouseDragged
            || type == kCGEventScrollWheel
            || type == kCGEventOtherMouseDown
            || type == kCGEventOtherMouseUp
            || type == kCGEventOtherMouseDragged;
      }


      CGEventMask input_event_mask()
      {
         return CGEventMaskBit(kCGEventKeyDown)
            | CGEventMaskBit(kCGEventKeyUp)
            | CGEventMaskBit(kCGEventFlagsChanged)
            | CGEventMaskBit(kCGEventLeftMouseDown)
            | CGEventMaskBit(kCGEventLeftMouseUp)
            | CGEventMaskBit(kCGEventRightMouseDown)
            | CGEventMaskBit(kCGEventRightMouseUp)
            | CGEventMaskBit(kCGEventMouseMoved)
            | CGEventMaskBit(kCGEventLeftMouseDragged)
            | CGEventMaskBit(kCGEventRightMouseDragged)
            | CGEventMaskBit(kCGEventScrollWheel)
            | CGEventMaskBit(kCGEventOtherMouseDown)
            | CGEventMaskBit(kCGEventOtherMouseUp)
            | CGEventMaskBit(kCGEventOtherMouseDragged);
      }

   } // namespace


   MacosInputBlocker::MacosInputBlocker() :
      m_eventtap(nullptr),
      m_runloopsource(nullptr),
      m_timeInterval(0),
      m_keyboardBlocking(false),
      m_mouseBlocking(false),
      m_softKeyboardBlocking(false),
      m_softMouseBlocking(false)
   {
   }


   MacosInputBlocker::~MacosInputBlocker()
   {
      destroyEventTap();
   }


   void MacosInputBlocker::initialize_input_blocker(::subsystem::LogWriter * plogwriter)
   {
      ::remoting_rfb::InputBlocker::initialize_input_blocker(plogwriter);
      createEventTap();
   }


   void MacosInputBlocker::setKeyboardBlocking(bool block)
   {
      critical_section_lock lock(&m_criticalsection);
      m_keyboardBlocking = block;
   }


   void MacosInputBlocker::setMouseBlocking(bool block)
   {
      critical_section_lock lock(&m_criticalsection);
      m_mouseBlocking = block;
   }


   void MacosInputBlocker::setSoftKeyboardBlocking(bool block, ::u32 timeInterval)
   {
      critical_section_lock lock(&m_criticalsection);
      m_softKeyboardBlocking = block;
      if (block)
      {
         m_timeInterval = timeInterval;
      }
   }


   void MacosInputBlocker::setSoftMouseBlocking(bool block, ::u32 timeInterval)
   {
      critical_section_lock lock(&m_criticalsection);
      m_softMouseBlocking = block;
      if (block)
      {
         m_timeInterval = timeInterval;
      }
   }


   class ::time MacosInputBlocker::getLastInputTime() const
   {
      critical_section_lock lock(&m_criticalsection);
      return m_lastInputTime;
   }


   void MacosInputBlocker::correctLastTime(const class ::time & time)
   {
      critical_section_lock lock(&m_criticalsection);
      if (time > m_lastInputTime)
      {
         m_lastInputTime = time;
      }
   }


   bool MacosInputBlocker::isRemoteInputAllowed()
   {
      critical_section_lock lock(&m_criticalsection);
      return !(m_softKeyboardBlocking || m_softMouseBlocking)
         || m_lastInputTime.elapsed() >= m_timeInterval;
   }


   CGEventRef MacosInputBlocker::eventTapCallback(
      CGEventTapProxy,
      CGEventType type,
      CGEventRef event,
      void * context)
   {
      auto inputblocker = static_cast<MacosInputBlocker *>(context);
      return inputblocker->processEvent(type, event);
   }


   CGEventRef MacosInputBlocker::processEvent(CGEventType type, CGEventRef event)
   {
      if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput)
      {
         if (m_eventtap)
         {
            CGEventTapEnable(m_eventtap, true);
         }
         return event;
      }

      const auto processId =
         CGEventGetIntegerValueField(event, kCGEventSourceUnixProcessID);
      if (processId == getpid())
      {
         return event;
      }

      const auto keyboardEvent = is_keyboard_event(type);
      const auto mouseEvent = is_mouse_event(type);

      critical_section_lock lock(&m_criticalsection);
      if ((keyboardEvent && m_softKeyboardBlocking)
         || (mouseEvent && m_softMouseBlocking))
      {
         m_lastInputTime.Now();
      }

      if ((keyboardEvent && m_keyboardBlocking)
         || (mouseEvent && m_mouseBlocking))
      {
         return nullptr;
      }

      return event;
   }


   void MacosInputBlocker::createEventTap()
   {
      if (m_eventtap)
      {
         return;
      }

      m_eventtap = CGEventTapCreate(
         kCGSessionEventTap,
         kCGHeadInsertEventTap,
         kCGEventTapOptionDefault,
         input_event_mask(),
         &MacosInputBlocker::eventTapCallback,
         this);
      if (!m_eventtap)
      {
         return;
      }

      m_runloopsource = CFMachPortCreateRunLoopSource(nullptr, m_eventtap, 0);
      if (!m_runloopsource)
      {
         CFRelease(m_eventtap);
         m_eventtap = nullptr;
         return;
      }

      CFRunLoopAddSource(CFRunLoopGetMain(), m_runloopsource, kCFRunLoopCommonModes);
      CGEventTapEnable(m_eventtap, true);
   }


   void MacosInputBlocker::destroyEventTap()
   {
      if (m_runloopsource)
      {
         CFRunLoopRemoveSource(CFRunLoopGetMain(), m_runloopsource, kCFRunLoopCommonModes);
         CFRelease(m_runloopsource);
         m_runloopsource = nullptr;
      }

      if (m_eventtap)
      {
         CFMachPortInvalidate(m_eventtap);
         CFRelease(m_eventtap);
         m_eventtap = nullptr;
      }
   }

} // namespace remoting_rfb_macos
