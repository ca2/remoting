#include "framework.h"
#include "remoting/remoting_rfb_macos/desktop/MacosScreenDriver.h"

#include <ApplicationServices/ApplicationServices.h>
#include <cmath>


namespace remoting_rfb_macos
{

   MacosScreenDriver::MacosScreenDriver() :
      m_pcriticalsectionFramebuffer(nullptr),
      m_detectionEnabled(false)
   {
   }


   MacosScreenDriver::~MacosScreenDriver()
   {
      terminateDetection();
   }


   void MacosScreenDriver::initialize_screen_driver(
      ::remoting_rfb_node::Configurator * pconfigurator,
      ::remoting_rfb::UpdateKeeper * pupdatekeeper,
      ::remoting_rfb::UpdateListener * pupdatelistener,
      ::innate_subsystem::Framebuffer * pframebuffer,
      lockable_critical_section * pcriticalsectionFramebuffer,
      ::subsystem::LogWriter * plogwriter)
   {
      m_pcriticalsectionFramebuffer = pcriticalsectionFramebuffer;
      m_screengrabber.initialize_screen_grabber();
      m_poller.initialize_poller(
         pconfigurator,
         pupdatekeeper,
         pupdatelistener,
         &m_screengrabber,
         pframebuffer,
         pcriticalsectionFramebuffer,
         plogwriter);
   }


   void MacosScreenDriver::executeDetection()
   {
      if (!m_detectionEnabled)
      {
         m_detectionEnabled = true;
         m_poller.resumeThread();
      }
   }


   void MacosScreenDriver::terminateDetection()
   {
      if (m_detectionEnabled)
      {
         m_poller.setThreadToFinish();
         m_poller.waitThreadToFinish();
         m_detectionEnabled = false;
      }
   }


   ::i32_size MacosScreenDriver::getScreenDimension()
   {
      critical_section_lock lock(m_pcriticalsectionFramebuffer);
      return m_screengrabber.getScreenRect().size();
   }


   bool MacosScreenDriver::grabFb(const ::i32_rectangle & rectangle)
   {
      critical_section_lock lock(m_pcriticalsectionFramebuffer);
      return m_screengrabber.grab(rectangle);
   }


   ::innate_subsystem::Framebuffer * MacosScreenDriver::getScreenBuffer()
   {
      return m_screengrabber.getScreenBuffer();
   }


   bool MacosScreenDriver::getScreenPropertiesChanged()
   {
      critical_section_lock lock(m_pcriticalsectionFramebuffer);
      return m_screengrabber.getPropertiesChanged();
   }


   bool MacosScreenDriver::getScreenSizeChanged()
   {
      critical_section_lock lock(m_pcriticalsectionFramebuffer);
      return m_screengrabber.getScreenSizeChanged();
   }


   bool MacosScreenDriver::applyNewScreenProperties()
   {
      critical_section_lock lock(m_pcriticalsectionFramebuffer);
      return m_screengrabber.applyNewProperties();
   }


   bool MacosScreenDriver::grabCursorShape(const ::innate_subsystem::PixelFormat &)
   {
      m_cursorshape.resetToEmpty();
      return true;
   }


   const ::remoting_rfb::CursorShape * MacosScreenDriver::getCursorShape()
   {
      return &m_cursorshape;
   }


   ::i32_point MacosScreenDriver::getCursorPosition()
   {
      auto event = CGEventCreate(nullptr);
      if (!event)
      {
         return {};
      }

      const auto location = CGEventGetLocation(event);
      CFRelease(event);
      const auto bounds = m_screengrabber.getScreenRect();
      return {
         (::i32) std::lround(location.x) - bounds.left,
         (::i32) std::lround(location.y) - bounds.top
      };
   }


   void MacosScreenDriver::getCopiedRegion(::i32_rectangle & rectangleCopy, ::i32_point & pointSource)
   {
      rectangleCopy.clear();
      pointSource = {};
   }


   ::remoting_rfb::Region MacosScreenDriver::getVideoRegion()
   {
      return {};
   }

} // namespace remoting_rfb_macos
