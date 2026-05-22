#pragma once


#include "apex/platform/application.h"


namespace remoting_rdx_client
{


   class CLASS_DECL_REMOTING_RDX_CLIENT application :
      virtual public ::apex::application
   {
   public:
      //::pointer<::app_app::main_window> m_pmainwindow;

      bool m_bStartedRdxClient = false;

      application();
      ~application() override;
      
      __DECLARE_APPLICATION_RELEASE_TIME();



      virtual void on_request(::request * prequest) override;




   };


} // namespace remoting_rdx_client



