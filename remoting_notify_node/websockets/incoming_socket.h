#pragma once


#include "app-core/library/application.h"
#include "apex/platform/app_consumer.h"
#include "apex/networking/sockets/httpd/incoming_socket.h"


namespace install
{


   class application;
   class service;


   class CLASS_DECL_APP_CORE_LIBRARY incoming_socket :
      virtual public app_consumer < ::library::application, ::httpd::incoming_socket >
   {
   public:


      //bootstrap * m_pbootstrap;


      incoming_socket();
      ~incoming_socket() override;

      //virtual void on_send_response() override;


   };


} // namespace install


