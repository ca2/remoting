#pragma once


#include "apex/platform/application.h"


namespace remoting_rdx_client
{


   class CLASS_DECL_REMOTING_RDX_CLIENT application :
      virtual public ::apex::application
   {
   public:
      

      ::string m_strHost;


      ::pointer < rdx_client > m_prdxclient;


      application();
      ~application() override;
      
      __DECLARE_APPLICATION_RELEASE_TIME();


      virtual void on_request(::request * prequest) override;


      virtual void defer_start_rdx_client();


      virtual void rdx_client_main();


      static LRESULT CALLBACK s_window_procedure_redirect(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

   };


} // namespace remoting_rdx_client



