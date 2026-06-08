#include "framework.h"
#include "remoting/remoting_rfb_macos/desktop/MacosScreenGrabber.h"

#include <ApplicationServices/ApplicationServices.h>
#include <cmath>


namespace remoting_rfb_macos
{

   namespace
   {

      ::i32_rectangle virtual_display_bounds()
      {
         CGDirectDisplayID displays[64] = {};
         uint32_t count = 0;
         ::i32_rectangle rectangle;
         rectangle.clear();

         if (CGGetActiveDisplayList(64, displays, &count) != kCGErrorSuccess || count == 0)
         {
            return rectangle;
         }

         auto first = true;
         for (uint32_t i = 0; i < count; ++i)
         {
            const auto bounds = CGDisplayBounds(displays[i]);
            ::i32_rectangle displayRectangle(
               (::i32) std::lround(bounds.origin.x),
               (::i32) std::lround(bounds.origin.y),
               (::i32) std::lround(bounds.origin.x + bounds.size.width),
               (::i32) std::lround(bounds.origin.y + bounds.size.height));

            if (first)
            {
               rectangle = displayRectangle;
               first = false;
            }
            else
            {
               rectangle.unite(displayRectangle);
            }
         }

         return rectangle;
      }

      ::innate_subsystem::PixelFormat macos_pixel_format()
      {
         ::innate_subsystem::PixelFormat pixelFormat;
         pixelFormat.initBigEndianByNative();
         pixelFormat.bitsPerPixel = 32;
         pixelFormat.colorDepth = 24;
         pixelFormat.redMax = 0xff;
         pixelFormat.greenMax = 0xff;
         pixelFormat.blueMax = 0xff;
         pixelFormat.redShift = 16;
         pixelFormat.greenShift = 8;
         pixelFormat.blueShift = 0;
         return pixelFormat;
      }

   } // namespace


   MacosScreenGrabber::MacosScreenGrabber()
   {
   }


   MacosScreenGrabber::~MacosScreenGrabber()
   {
   }


   void MacosScreenGrabber::initialize_screen_grabber()
   {
      applyNewProperties();
   }


   bool MacosScreenGrabber::grab(const ::i32_rectangle &)
   {
      if (getPropertiesChanged())
      {
         return false;
      }

      const auto bounds = virtual_display_bounds();
      const CGRect captureBounds = CGRectMake(bounds.left, bounds.top, bounds.width(), bounds.height());
      auto image = CGWindowListCreateImage(
         captureBounds,
         kCGWindowListOptionOnScreenOnly,
         kCGNullWindowID,
         kCGWindowImageDefault);

      if (!image)
      {
         return false;
      }

      const auto dimension = m_pframebufferWork->getDimension();
      auto colorSpace = CGColorSpaceCreateDeviceRGB();
      auto context = CGBitmapContextCreate(
         m_pframebufferWork->getBuffer(),
         dimension.cx,
         dimension.cy,
         8,
         m_pframebufferWork->getBytesPerRow(),
         colorSpace,
         (CGBitmapInfo) kCGBitmapByteOrder32Little | (CGBitmapInfo) kCGImageAlphaNoneSkipFirst);

      CGColorSpaceRelease(colorSpace);

      if (!context)
      {
         CGImageRelease(image);
         return false;
      }

      CGContextDrawImage(context, CGRectMake(0, 0, dimension.cx, dimension.cy), image);

      CGContextRelease(context);
      CGImageRelease(image);
      return true;
   }


   bool MacosScreenGrabber::getPropertiesChanged()
   {
      return getScreenSizeChanged() || getPixelFormatChanged();
   }


   bool MacosScreenGrabber::getPixelFormatChanged()
   {
      return m_pframebufferWork->getPixelFormat() != macos_pixel_format();
   }


   bool MacosScreenGrabber::getScreenSizeChanged()
   {
      return virtual_display_bounds() != m_rectangleFullScreen;
   }


   bool MacosScreenGrabber::applyNewFullScreenRect()
   {
      m_rectangleFullScreen = virtual_display_bounds();
      return m_rectangleFullScreen.is_set();
   }


   bool MacosScreenGrabber::applyNewPixelFormat()
   {
      defer_construct_newø(m_pframebufferWork);
      return m_pframebufferWork->setProperties(m_rectangleFullScreen, macos_pixel_format());
   }


   bool MacosScreenGrabber::applyNewProperties()
   {
      return applyNewFullScreenRect() && applyNewPixelFormat();
   }

} // namespace remoting_rfb_macos
