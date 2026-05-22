//
// Created by camilo on 2026-02-12 02:03 <3ThomasBorregaardSørensen!!
//
#include "framework.h"
#include "application.h"
#include "Server.h"
#include "ServerApplication.h"
#include "acme/constant/id.h"
#include "acme/handler/request.h"
#include "acme/platform/system.h"
#include "remoting/rfb_client/remoting.h"
#include "remoting/remoting_rfb/platform/remoting.h"
#include "Service.h"
//#include "acme/_operating_system.h"
//#include "main_window.h"


__IMPLEMENT_APPLICATION_RELEASE_TIME(remoting_rfb_node_desktop);
IMPLEMENT_APPLICATION_FACTORY(remoting_rfb_node_desktop);


//int remoting_impact_main(::particle * pparticle, const ::file::path & path);
//CLASS_DECL_ACME HMODULE GetModuleFromFunction(void* pFunc);


namespace remoting_rfb_node_desktop
{

   void function_at_remoting_remoting()
   {
   }

   application::application()
   {

      ::remoting_rfb::defer_initialize_remoting();

      //HINSTANCE hInstance = (HINSTANCE)GetModuleFromFunction(&function_at_remoting_remoting);

      // return hInstance;

      //::system()->m_premoting->m_pHinstance = (void *) hInstance;

      ::system()->m_bFinalizeIfNoSession = false;

      m_bNetworking = true;

      m_strAppId = "remoting/node_desktop";

      m_bUserApplication = false;

   }


   application::~application()
   {

   }


   void application::init_instance()
   {
   
      ::apex::application::init_instance();
      
      auto pfactory = system()->factory("remoting", OPERATING_SYSTEM_NAME);

      pfactory->merge_to_global_factory();
      
   }


   void application::on_request(::request * prequest)
   {

      auto ecommand = prequest->m_ecommand;

//      if (ecommand == e_command_application_start)
//      {
//
//
//         auto pfactory = system()->factory("remoting", "windows");
//
//         pfactory->merge_to_global_factory();
//         //defer_construct_newø(m_pserverapplication);///
//
//         //m_pserverapplication->on_start();
//
//      }
//      else
      
      if (ecommand == e_command_file_open)
      {

         auto path = prequest->m_payloadFile.as_file_path();

         m_bOpenFile = true;

         main_node(path);

         //fork([this, path]()
         //{
           // remoting_impact_main(path);

         //});

      }
      else if (ecommand == e_command_default_start)
      {

         if (!m_bOpenFile)
         {

            main_node({});

         }

      }
      
   }


   bool application::check_pipe_node_client_executable_paths(const file::path& pathNode, const file::path& pathClient)
   {

      auto pathNodeFolder = pathNode.folder();

      auto pathClientFolder = pathClient.folder();

      if (pathNodeFolder.is_empty() || pathClientFolder.is_empty())
      {

         return false;

      }

      if (pathNodeFolder != pathClientFolder)
      {

         return false;

      }

      ::string strNodeName = pathNode.name();

      ::string strClientName = pathClient.name();

      if (!strNodeName.ends_eat(".exe"))
      {

         return false;

      }

      if (!strClientName.ends_eat(".exe"))
      {

         return false;

      }

      bool bEaten = strClientName.ends_eat("_control");

      if (strNodeName != strClientName)
      {

         return false;

      }

      return true;

   }


   lresult application::handle_direct_id(const enum_id eid, wparam wparam, lparam lparam)
   {

      //Server().handle_direct_id(eid, wparam, lparam);

      return ::platform::application::handle_direct_id(eid, wparam, lparam);

   }


   Server & application::Server()
   {

      if (m_pservice)
      {

         return *m_pservice->m_pserver;

      }
      else if (m_pserverapplication)
      {

         return *m_pserverapplication->m_pserver;

      }

      throw ::exception(error_wrong_state);

      // if (!m_pserver)
      // {
      //    construct_newø(m_pserver);
      //    m_pserver->initialize_remoting_node_desktop_server(false, nullptr, nullptr, this);
      // }
      //

      return *m_pserver;

   }


} // namespace remoting_rfb_node_desktop
