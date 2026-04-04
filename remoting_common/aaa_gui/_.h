// Into tightvnc sources by Camilo on 2026-02-10 <3ThomasBorregaardSorensen!!
#pragma once



#include "remoting/_.h"


#if defined(_remoting_gui_project)
#define CLASS_DECL_REMOTING_GUI  CLASS_DECL_EXPORT
#else
#define CLASS_DECL_REMOTING_GUI  CLASS_DECL_IMPORT
#endif



namespace remoting
{

   class DeviceContext;


}





