// From rfb_client by camilo on 2026-06-04 10:30 BRT <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
#pragma once


#include "subsystem/node/OperatingSystemApplication.h"


namespace remoting_rfb_node_desktop
{


   class CLASS_DECL_REMOTING_RFB_NODE_DESKTOP OperatingSystemApplication :
      virtual public ::subsystem::OperatingSystemApplication
   {
   public:

      OperatingSystemApplication();
      ~OperatingSystemApplication() override;


   };


} // namespace remoting_rfb_node_desktop
