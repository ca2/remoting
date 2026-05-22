#include "framework.h"
#include "application.h"
#include "acme/platform/system.h"


__IMPLEMENT_APPLICATION_RELEASE_TIME(remoting_rdx_client);
IMPLEMENT_APPLICATION_FACTORY(remoting_rdx_client);
int rdx_client_main();

namespace remoting_rdx_client
{

   #define _CRT_SECURE_NO_WARNINGS

   // __IMPLEMENT_APPLICATION_RELEASE_TIME1(app_app));


   application::application()
   {
      
      m_strAppId = "remoting/rdx_client";

      m_strAppName = "remoting/rdx_client";

      m_strBaseSupportId = "remoting/rdx_client";

      m_bNetworking = false;

      m_bLicense = false;

      m_bImaging = false; // showing application icon may use innate_ui icon?

      m_bWriteText = false;

      m_bInterprocessCommunication = false;

   }


   application::~application()
   {

   }


   void application::on_request(::request * prequest)
   {

      if (!m_bStartedRdxClient)
      {
         m_bStartedRdxClient = true;
         fork(
            [this]()
            {
               rdx_client_main();
            });
      }

      //if (!m_pmainwindow)
      //{

      //   m_pmainwindow = create_newø<::app_app::main_window>();

      //   m_pmainwindow->create_main_window(prequest);

      //   //m_pmainwindow->display(e_display_normal);
      //   
      //   m_pmainwindow->initial_frame_placement();

      //}

//      pmainwindow->m_procedureOnAfterCreate = [pmainwindow]()
//      {
//
//         //pmainwindow->set_need_layout();
//
//         pmainwindow->set_need_redraw();
//
//         pmainwindow->post_redraw();
//
//      };

      //pmainwindow->create_main_window(prequest);

      //pmainwindow->m_pthreadUserInteraction->m_procedurea.add([this]() {});

      //pmainwindow->m_pthreadUserInteraction->run_posted_procedures();

   }


  
} // namespace remoting_rdx_client



