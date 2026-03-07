#pragma once


#include "apex/networking/sockets/httpd/incoming_socket_thread.h"


namespace remoting_notify_node
{


   namespace websockets
   {

   
      class service;
      class socket;


      class CLASS_DECL_REMOTING_NOTIFY_NODE incoming_socket_thread :
      virtual public ::httpd::incoming_socket_thread
      {
      public:
         
         
         incoming_socket_thread();
         ~incoming_socket_thread() override;
         
         
         void initialize(::particle * pparticle) override;
         
         void install_message_routing(::channel * pchannel) override;
         
         //::pointer < ::netserver::socket_handler > create_socket_handler() override;
         //::pointer < ::sockets::listen_socket_base > create_listen_socket() override;
         
         
      };


   } // namespace websockets


} // namespace remoting_notify_node



