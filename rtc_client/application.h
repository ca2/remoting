//
// Created by camilo on 2026-02-12 01:59 <3ThomasBorregaardSørensen!!
//

#pragma once


#include "apex/platform/application.h"
#include "subsystem/node/OperatingSystemApplication.h"


namespace remoting_client
{


   class remoting;

   class CLASS_DECL_REMOTING_CLIENT application :
   virtual public ::apex::application
   {
   public:

      bool m_bOpenFile = false;

      ::pointer<remoting> m_premoting;
      ::pointer < ::subsystem::OperatingSystemApplicationInterface > m_poperatingsystemapplication;

      __DECLARE_APPLICATION_RELEASE_TIME();

      application();
      ~application() override;

      void init_instance() override;
      
      void on_request(::request * prequest) override;
      
      
      ::remoting_client::remoting * remoting();


      ::lresult handle_direct_id(const ::enum_id eid, ::wparam wparam, ::lparam lparam) override;


   };

} // namespace remoting_client


