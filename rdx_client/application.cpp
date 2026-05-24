#include "framework.h"
#include "application.h"
#include "acme/filesystem/filesystem/file_context.h"
#include "acme/handler/request.h"
#include "acme/operating_system/windows/window.h"
#include "acme/platform/system.h"
#include "acme_windowing_win32/_.h"
#include "acme_windowing_win32/windowing.h"
#include "main_rdp_host.h"
#include "rdx_client.h"


__IMPLEMENT_APPLICATION_RELEASE_TIME(remoting_rdx_client);
IMPLEMENT_APPLICATION_FACTORY(remoting_rdx_client);
#include "rdp_host2.h"
#include "rdp_host4.h"
#include "rdp_host5.h"



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


   LRESULT CALLBACK application::s_window_procedure_redirect(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {

      return ::windows::window::s_window_procedure(hwnd, msg, wParam, lParam);

   }

   // void application::on_system_main()
   // {
   //
   //    ::cast < ::win32::acme::windowing::windowing > pacmewindowingwindowing = system()->acme_windowing();
   //
   //    pacmewindowingwindowing->_register_acme_nano_window_class(::system()->m_hinstanceThis);
   //
   //    pacmewindowingwindowing->_register_com_host_window_class(::system()->m_hinstanceThis);
   //    fork ([this]()
   //       {
   //
   //    rdp_host4::main();
   //
   //       });
   //    //rdx_client_main();
   //
   //    //rdp_host2::main();
   //
   //
   //
   // }


   void application::on_request(::request * prequest)
   {

      if (prequest->m_ecommand == e_command_file_open)
      {

         auto ini = file()->get_ini(prequest->m_payloadFile);

         ::string strHost = ini["host"];

         if (strHost.is_empty())
         {

            send_message_box("host: not specified", "host not specified");

            set_finish();

            return;

         }

         defer_start_rdx_client(strHost);
         //::cast < ::win32::acme::windowing::windowing > pacmewindowingwindowing = system()->acme_windowing();

         //pacmewindowingwindowing->_register_acme_nano_window_class(::system()->m_hinstanceThis);

         //pacmewindowingwindowing->_register_com_host_window_class(::system()->m_hinstanceThis);
         // fork ([this]()
         //    {
         //
         // //rdp_host4::main();
         //
         //    });
      }
      else
      {

         send_message_box("Usage: specify .remoting_rdx file to open", "No file specified");

         set_finish();

      }

      //defer_start_rdx_client();

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


   void application::defer_start_rdx_client(const ::scoped_string & scopedstrHost)
   {

      if (!m_prdxclient)
      {

         ::string strHost(scopedstrHost);

         m_papplication->fork([this, strHost]()
         {

            ::task_set_name("rdxclientwnd");

            rdx_client_main(strHost);



         });

      }


   }




   void application::rdx_client_main(const ::scoped_string & scopedstrHost)
   {

      _defer_co_initialize_ex(false);




      construct_newø(m_prdxclient);

      //rdp_host5::main();

       m_prdxclient->main_window_main(scopedstrHost);


      set_finish();

   }

} // namespace remoting_rdx_client



