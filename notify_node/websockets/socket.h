#pragma once


#include "apex/platform/app_consumer.h"
#include "apex/networking/netserver/socket.h"
#include "aura/user/user/notification_listener.h"


namespace remoting_notify_node
{

   
   namespace websockets
   {
   
   
      class application;
      class service;


      class CLASS_DECL_REMOTING_NOTIFY_NODE socket :
      virtual public app_consumer < application,  ::netserver::socket,::user::notification_listener >
      {
      public:
         
         
         //bootstrap *          m_pbootstrap;
         
         
         socket();
         virtual ~socket();
         
         
         void on_initialize_particle() override;
         
         virtual void on_send_response() override;
         
         void on_keyboard_layout_change(const char * pszKeyboardLayoutId) override;
         
         
         void on_websocket_data(const ::scoped_string & scopedstr) override;
       
         
      };


   } // namespace websockets


} // namespace remoting_notify_node



