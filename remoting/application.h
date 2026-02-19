//
// Created by camilo on 2026-02-12 01:59 <3ThomasBorregaardSørensen!!
//

#pragma once


#include "acme/platform/application.h"
#include "remoting/remoting/ConnectingDialog.h"
namespace remoting_remoting
{

   class application :
   virtual public ::platform::application
   {
   public:
      ConnectingDialog m_connectingdialog;
LogWriter * m_logWriter;
      bool m_bOpenFile = false;

      __DECLARE_APPLICATION_RELEASE_TIME();

      application();
      ~application() override;
      int remoting_impact_main(const ::file::path &path);

      void on_request(::request * prequest) override;


      ::lresult handle_direct_id(const ::enum_id eid, ::wparam wparam, ::lparam lparam) override;


   };

} // namespace remoting_remoting


