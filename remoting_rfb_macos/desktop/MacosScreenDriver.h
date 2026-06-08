#pragma once

#include "remoting/remoting_rfb_macos/_common_header.h"
#include "remoting/remoting_rfb/desktop/ScreenDriver.h"
#include "remoting/remoting_rfb/desktop/Poller.h"
#include "remoting/remoting_rfb_macos/desktop/MacosScreenGrabber.h"


namespace remoting_rfb_macos
{

   class CLASS_DECL_REMOTING_MACOS MacosScreenDriver :
      virtual public ::remoting_rfb::ScreenDriver
   {
   public:

      MacosScreenDriver();
      ~MacosScreenDriver() override;

      void initialize_screen_driver(
         ::remoting_rfb_node::Configurator * pconfigurator,
         ::remoting_rfb::UpdateKeeper * pupdatekeeper,
         ::remoting_rfb::UpdateListener * pupdatelistener,
         ::innate_subsystem::Framebuffer * pframebuffer,
         lockable_critical_section * pcriticalsectionFramebuffer,
         ::subsystem::LogWriter * plogwriter) override;

      void executeDetection() override;
      void terminateDetection() override;
      ::i32_size getScreenDimension() override;
      bool grabFb(const ::i32_rectangle & rectangle = {}) override;
      ::innate_subsystem::Framebuffer * getScreenBuffer() override;
      bool getScreenPropertiesChanged() override;
      bool getScreenSizeChanged() override;
      bool applyNewScreenProperties() override;
      bool grabCursorShape(const ::innate_subsystem::PixelFormat & pixelformat) override;
      const ::remoting_rfb::CursorShape * getCursorShape() override;
      ::i32_point getCursorPosition() override;
      void getCopiedRegion(::i32_rectangle & rectangleCopy, ::i32_point & pointSource) override;
      ::remoting_rfb::Region getVideoRegion() override;

   private:

      MacosScreenGrabber m_screengrabber;
      ::remoting_rfb::Poller m_poller;
      ::remoting_rfb::CursorShape m_cursorshape;
      lockable_critical_section * m_pcriticalsectionFramebuffer;
      bool m_detectionEnabled;

   };

} // namespace remoting_rfb_macos
