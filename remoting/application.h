//
// Created by camilo on 2026-02-12 01:59 <3ThomasBorregaardSørensen!!
//

#pragma once


#include "acme/platform/application.h"

namespace remoting_remoting
{

   class application :
   virtual public ::platform::application
   {
   public:

      __DECLARE_APPLICATION_RELEASE_TIME();

      application();
      ~application() override;


      void on_request(::request * prequest) override;


   };

} // namespace remoting_remoting


