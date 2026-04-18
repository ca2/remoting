#pragma once


#include "aura/platform/application.h"


namespace remoting_remote_connect
{


   class CLASS_DECL_REMOTING_REMOTE_CONNECT application :
      virtual public ::aura::application
   {
   public:
      ::pointer<::remoting_remote_connect::main_window> m_pmainwindow;

      application();
      ~application() override;
      
      __DECLARE_APPLICATION_RELEASE_TIME();



      virtual void on_request(::request * prequest) override;


      string draw2d_get_default_implementation_name() override;


   };


} // namespace remoting_remote_connect



