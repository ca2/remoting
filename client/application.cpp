//
// Created by camilo on 2026-02-12 02:03 <3ThomasBorregaardSørensen!!
//
#include "framework.h"
#include "application.h"
#include "acme/constant/id.h"
#include "acme/handler/request.h"
#include "acme/platform/system.h"
#include "remoting/client/remoting.h"
#include "remoting/remoting/platform/remoting.h"
#include "subsystem/socket/Sockets.h"
#include "resource.h"
//#include "acme/_operating_system.h"
//#include "main_window.h"


__IMPLEMENT_APPLICATION_RELEASE_TIME(remoting_client);
IMPLEMENT_APPLICATION_FACTORY(remoting_client);

//int remoting_impact_main(::particle * pparticle, const ::file::path & path);
//CLASS_DECL_ACME HMODULE GetModuleFromFunction(void* pFunc);


namespace remoting_client
{

   void function_at_remoting_remoting()
   {
   }

   application::application()
   {

      //HINSTANCE hInstance = (HINSTANCE)GetModuleFromFunction(&function_at_remoting_remoting);

      // return hInstance;

      //::system()->m_premoting->m_pHinstance = (void *) hInstance;

      ::system()->m_bFinalizeIfNoSession = false;

      m_bNetworking = true;

      m_strAppId = "remoting/client";
   }

   application::~application()
   {

   }


   void application::init_instance()
   {
      
      ::apex::application::init_instance();
      
      setResourceName(IDD_LOGINDIALOG, "IDD_LOGINDIALOG");
      setResourceName(IDC_CSERVER, "IDC_CSERVER");
      setResourceName(IDD_DAUTH, "IDD_DAUTH");
      setResourceName(IDC_EHOST, "IDC_EHOST");
      setResourceName(IDC_EPASSW, "IDC_EPASSW");
      setResourceName(IDD_CONNECTING, "IDD_CONNECTING");
      setResourceName(IDC_HOST, "IDC_HOST");
      setResourceName(IDC_STATUS, "IDC_STATUS");
      setResourceName(IDC_PROGRESS1, "IDC_PROGRESS1");
      setResourceName(IDD_FS_WARNING, "IDD_FS_WARNING");
      setResourceName(IDC_CWARN_FS, "IDC_CWARN_FS");
      
   }

   
   ::remoting_client::remoting * application::remoting()
   {
      
      if(!m_premoting)
      {
         
         MainSubsystem().Sockets().startSockets();

         ::remoting::defer_initialize_remoting();

         constructø(m_poperatingsystemapplication);

         m_poperatingsystemapplication->initialize_operating_system_application();

         defer_construct_newø(m_premoting);

         m_premoting->on_start();

      }

      return m_premoting;
      
   }


   void application::on_request(::request * prequest)
   {
      
      auto ecommand = prequest->m_ecommand;

      if (ecommand == e_command_default_start)
      {
         
         if (!m_bOpenFile)
         {

            remoting()->open_file({});

         }

      }
      else if (ecommand == e_command_file_open)
      {

         auto path = prequest->m_payloadFile.as_file_path();

         m_bOpenFile = true;
         
         auto premoting = remoting();

         premoting->open_file(path);

         //fork([this, path]()
         //{
           // remoting_impact_main(path);

         //});

      }
      else if (ecommand == e_command_default_start)
      {


      }
   }


   lresult application::handle_direct_id(const enum_id eid, wparam wparam, lparam lparam)
   {

      m_premoting->handle_direct_id(eid, wparam, lparam);

      return ::platform::application::handle_direct_id(eid, wparam, lparam);

   }


} // namespace remoting_client
