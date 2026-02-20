//
// Created by camilo on 2026-02-14 23:03 <3ThomasBorregaardSørensen!!
//
#pragma once


#include "acme/_.h"



#if defined(_remoting_common_project)
#define CLASS_DECL_REMOTING_COMMON CLASS_DECL_EXPORT
#else
#define CLASS_DECL_REMOTING_COMMON CLASS_DECL_IMPORT
#endif

using LogWriter = ::particle;




namespace remoting
{


   class DeviceContext;

}