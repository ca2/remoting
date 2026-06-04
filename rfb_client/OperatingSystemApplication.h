// Created by camilo on 2026-06-04 02:01 BRT <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
#pragma once


#include "subsystem/node/OperatingSystemApplication.h"


namespace remoting_rfb_client
{


   class CLASS_DECL_REMOTING_RFB_CLIENT OperatingSystemApplication :
      virtual public ::subsystem::OperatingSystemApplication
   {
   public:

      OperatingSystemApplication();
      ~OperatingSystemApplication() override;


   };


} // namespace remoting_rfb_client
