#pragma once


#include "apex/platform/app_consumer.h"
#include "apex/networking/netserver/socket.h"

namespace remoting_notify_node
{

   
   namespace websockets
   {
   
   
      class application;
      class service;


      class CLASS_DECL_REMOTING_NOTIFY_NODE socket :
      virtual public app_consumer < application,  ::netserver::socket >
      {
      public:
         
         
         //bootstrap *          m_pbootstrap;
         
         
         socket();
         virtual ~socket();
         
         virtual void on_send_response() override;
         
         
      };


   } // namespace websockets


} // namespace remoting_notify_node



