//
// Created by camilo on 2026-02-14 23:03 <3ThomasBorregaardSørensen!!
//

///TBS__<3tbs
#pragma once



#include "apex/_.h"



#if defined(_remoting_common_project)
#define CLASS_DECL_REMOTING_COMMON CLASS_DECL_EXPORT
#else
#define CLASS_DECL_REMOTING_COMMON CLASS_DECL_IMPORT
#endif

//
//#include "acme/_.h"
////#if defined(_STDIO_H_)
////#error "already included?!?! WHAT?!?! (At apex:a)"
////#endif
//
//#undef PLATFORM_LAYER_NAME
//#define PLATFORM_LAYER_NAME apex
//
//#if defined(_apex_project)
//#define CLASS_DECL_APEX  CLASS_DECL_EXPORT
//#else
//#define CLASS_DECL_APEX  CLASS_DECL_IMPORT
//#endif
//
//
////#define CA2_APEX
//

#ifdef __cplusplus


using LogWriter = ::particle;




namespace remoting
{


   class DeviceContext;

}


#endif

