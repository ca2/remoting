#pragma once

#include "remoting/remoting_rfb_macos/_common_header.h"
#include "remoting/remoting_rfb/desktop/ScreenGrabber.h"


namespace remoting_rfb_macos
{

   class CLASS_DECL_REMOTING_MACOS MacosScreenGrabber :
      virtual public ::remoting_rfb::ScreenGrabber
   {
   public:

      MacosScreenGrabber();
      ~MacosScreenGrabber() override;

      void initialize_screen_grabber();

      bool grab(const ::i32_rectangle & rectangle = {}) override;
      bool getPropertiesChanged() override;
      bool getPixelFormatChanged() override;
      bool getScreenSizeChanged() override;
      bool applyNewFullScreenRect() override;
      bool applyNewPixelFormat() override;
      bool applyNewProperties() override;

   };

} // namespace remoting_rfb_macos
