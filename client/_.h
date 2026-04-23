// Into tightvnc sources by Camilo on 2026-02-10 <3ThomasBorregaardSorensen!!
#pragma once



#include "remoting/remoting/_.h"


#if defined(_remoting_client_project)
#define CLASS_DECL_REMOTING_CLIENT  CLASS_DECL_EXPORT
#else
#define CLASS_DECL_REMOTING_CLIENT  CLASS_DECL_IMPORT
#endif


namespace remoting_client
{

   
   class keyboard_layout_change;

   class remoting;

   class ViewerWindow;

   class DesktopWindow;


} // namespace remoting_client



