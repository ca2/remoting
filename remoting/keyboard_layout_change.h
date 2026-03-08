// Created by camilo on 2026-03-08 04:06 <3ThomasBorregaardSørensen!!
#pragma once


#include "apex/networking/sockets/http/http_callback.h"


namespace remoting_remoting
{


   class CLASS_DECL_REMOTING_REMOTING keyboard_layout_change :
      virtual public ::sockets::http_callback
   {
   public:


      keyboard_layout_change();
      ~keyboard_layout_change() override;


      void on_websocket_text(const ::scoped_string& scopedstrText) override;


   };


} // namespace remoting_remoting



