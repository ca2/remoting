// Into remoting sources by Camilo on 2026-02-10 <3ThomasBorregaardSorensen!!
#pragma once



#include "remoting/remoting_rtc/_.h"
#include "acme/operating_system/windows_common/com/_.h"

#if defined(_remoting_rdx_client_project)
#define CLASS_DECL_REMOTING_RDX_CLIENT  CLASS_DECL_EXPORT
#else
#define CLASS_DECL_REMOTING_RDX_CLIENT  CLASS_DECL_IMPORT
#endif


// #define DISPID_CONNECTING 1
// #define DISPID_CONNECTED 2
// #define DISPID_DISCONNECTED 2
// #define DISPID_FATALERROR 3
// #define DISPID_LOGINCOMPLETE 4
// #define DISPID_ENTERFULLSCREENMODE 5
// #define DISPID_LEAVEFULLSCREENMODE 6
// #define DISPID_REQUEST_ENTERFULLSCREENMODE 8
// #define DISPID_REQUEST_LEAVEFULLSCREENMODE 9

namespace remoting_rdx_client
{

   class rdx_client;

   class main_window;
   
   // class keyboard_layout_change;
   //
   // class remoting;
   //
   // class ViewerWindow;
   //
   // class DesktopWindow;
   //

} // namespace remoting_client


namespace windows
{

   class in_place_frame;

   class in_place_site;

   class client_site;

   class com_window;

}// namespace windows

