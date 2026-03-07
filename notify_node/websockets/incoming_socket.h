#pragma once


#include "remoting/notify_node/websockets/application.h"
#include "apex/platform/app_consumer.h"
#include "apex/networking/sockets/httpd/incoming_socket.h"


namespace remoting_notify_node
{

   namespace websockets
   {
   
      class application;
      class service;
      
      
      class CLASS_DECL_REMOTING_NOTIFY_NODE incoming_socket :
      virtual public app_consumer < application, ::httpd::incoming_socket >
      {
      public:
         
         
         //bootstrap * m_pbootstrap;
         
         
         incoming_socket();
         ~incoming_socket() override;
         
         //virtual void on_send_response() override;
         
         
      };
   
   
   } // namespace websockets


} // namespace remoting_notify_node


