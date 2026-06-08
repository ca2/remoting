#pragma once

#include "remoting/remoting_rfb_macos/_common_header.h"
#include "remoting/remoting_rfb/desktop/ScreenDriverFactory.h"


namespace remoting_rfb_macos
{

   class CLASS_DECL_REMOTING_MACOS MacosScreenDriverFactory :
      virtual public ::remoting_rfb::ScreenDriverFactory
   {
   public:

      MacosScreenDriverFactory();
      ~MacosScreenDriverFactory() override;

      ::pointer < ::remoting_rfb::ScreenDriver > createScreenDriver(
         ::remoting_rfb_node::Configurator * pconfigurator,
         ::remoting_rfb::UpdateKeeper * pupdatekeeper,
         ::remoting_rfb::UpdateListener * pupdatelistener,
         ::innate_subsystem::Framebuffer * pframebuffer,
         lockable_critical_section * pcriticalsectionFramebuffer,
         ::subsystem::LogWriter * plogwriter) override;

   };

} // namespace remoting_rfb_macos
