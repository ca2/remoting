#pragma once


#include "apex/platform/app_consumer.h"
#include "apex/networking/netserver/socket.h"


namespace install
{


   class application;
   class service;


   class CLASS_DECL_APP_CORE_LIBRARY socket :
      virtual public app_consumer < ::library::application,  ::netserver::socket >
   {
   public:


      bootstrap *          m_pbootstrap;


      socket();
      virtual ~socket();

      virtual void on_send_response() override;


   };


} // namespace install


