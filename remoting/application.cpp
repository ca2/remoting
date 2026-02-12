//
// Created by camilo on 2026-02-12 02:03 <3ThomasBorregaardSørensen!!
//
#include "framework.h"
#include "application.h"
#include "acme/handler/request.h"
#include "acme/platform/system.h"

//#include "main_window.h"


__IMPLEMENT_APPLICATION_RELEASE_TIME(remoting_remoting);
IMPLEMENT_APPLICATION_FACTORY(remoting_remoting);

int remoting_impact_main();

namespace remoting_remoting
{

   application::application()
   {

   }

   application::~application()
   {

   }


   void application::on_request(::request * prequest)
   {

      if (prequest->m_ecommand == e_command_application_started)
      {

         fork([this]()
         {
            remoting_impact_main();
         });

      }

   }

} // namespace remoting_remoting
