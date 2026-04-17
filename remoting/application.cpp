//
// Created by camilo on 2026-02-12 02:03 <3ThomasBorregaardSørensen!!
//
#include "framework.h"
#include "application.h"
#include "acme/constant/id.h"
#include "acme/handler/request.h"
#include "acme/platform/system.h"
#include "remoting/remoting_common/remoting.h"
#include "remoting/remoting/remoting.h"
//#include "acme/_operating_system.h"
//#include "main_window.h"


__IMPLEMENT_APPLICATION_RELEASE_TIME(remoting_remoting);
IMPLEMENT_APPLICATION_FACTORY(remoting_remoting);

//int remoting_impact_main(::particle * pparticle, const ::file::path & path);
//CLASS_DECL_ACME HMODULE GetModuleFromFunction(void* pFunc);


namespace remoting_remoting
{

   void function_at_remoting_remoting()
   {
   }

   application::application()
   {

      ::remoting::defer_initialize_remoting();

      //HINSTANCE hInstance = (HINSTANCE)GetModuleFromFunction(&function_at_remoting_remoting);

      // return hInstance;

      //::system()->m_premoting->m_pHinstance = (void *) hInstance;

      ::system()->m_bFinalizeIfNoSession = false;

      m_bNetworking = true;

      m_strAppId = "remoting/remoting";
   }

   application::~application()
   {

   }


   void application::on_request(::request * prequest)
   {
      auto ecommand = prequest->m_ecommand;

      if (ecommand == e_command_application_start)
      {

         defer_construct_newø(m_premoting);

         m_premoting->on_start();

         

      }
      else if (ecommand == e_command_file_open)
      {

         auto path = prequest->m_payloadFile.as_file_path();
         m_bOpenFile = true;

         m_premoting->open_file(path);

         //fork([this, path]()
         //{
           // remoting_impact_main(path);

         //});

      }
      else if (ecommand == e_command_application_started)
      {

         if (!m_bOpenFile)
         {

            m_premoting->open_file({});

         }

      }
   }


   lresult application::handle_direct_id(const enum_id eid, wparam wparam, lparam lparam)
   {

      m_premoting->handle_direct_id(eid, wparam, lparam);

      return ::platform::application::handle_direct_id(eid, wparam, lparam);

   }


} // namespace remoting_remoting
