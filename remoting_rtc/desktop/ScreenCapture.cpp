// Created by camilo on 2026-05-21 16:20 <3ThomasBorregaardSørensen!!
#include "framework.h"
#include "ScreenCapture.h"


namespace remoting
{


   ScreenCapture::ScreenCapture()
   {

   }


   ScreenCapture::~ScreenCapture() {}


   void ScreenCapture::on_frame(bitmap * pbitmap)
   {
      // Process the frame data here
      // 'pbitmap' is a pointer to the bitmap structure
      // Example: Just print the frame information
      printf("Received frame: %dx%d, stride: %d\n", pbitmap->m_uWidth, pbitmap->m_uHeight, pbitmap->m_uStride);
      // You can also copy the data to your framebuffer or process it as needed
   }


} // namespace remoting
