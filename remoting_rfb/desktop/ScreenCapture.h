// Created by camilo on 2026-05-21 16:17 <3ThomasBorregaardSørensen!!
#pragma once


namespace remoting_rfb
{


   struct bitmap
   {
      ::u32    m_uWidth;
      ::u32    m_uHeight;
      ::u32    m_uStride;
      void *   m_pdata;
   };


   class CLASS_DECL_REMOTING_RFB ScreenCapture : virtual public ::particle
   {
   public:


   
      ScreenCapture();


      ~ScreenCapture() override;

      // brga8
      virtual void on_frame(bitmap * pbitmap);


   };


} // namespace remoting_windows
