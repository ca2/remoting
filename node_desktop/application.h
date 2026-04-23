//
// Created by camilo on 2026-02-12 01:59 <3ThomasBorregaardSørensen!!
//

#pragma once


#include "apex/platform/application.h"



namespace remoting_node_desktop
{


   //class remoting;

   class CLASS_DECL_REMOTING_NODE_DESKTOP application :
      virtual public ::apex::application
   {
   public:

      bool m_bOpenFile = false;

      ::pointer < Server > m_pserver;

      ///::pointer<remoting> m_premoting;

      __DECLARE_APPLICATION_RELEASE_TIME();

      application();
      ~application() override;

      void on_request(::request * prequest) override;


      ::lresult handle_direct_id(const ::enum_id eid, ::wparam wparam, ::lparam lparam) override;

      Server & Server();


   };

} // namespace remoting_node_desktop


