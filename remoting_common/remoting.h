//
// Created by camilo on 2026-02-15 21:52 <3ThomasBorregaardSørensen!!
//

#pragma once

#include "acme/_operating_system.h"
#include "acme/platform/remoting.h"

namespace remoting
{


//   class Environment;


   class CLASS_DECL_REMOTING_COMMON remoting :
   virtual public  ::platform::remoting
   {
   public:

      //::pointer < Environment > m_penvironment;


      remoting();
      ~remoting() override;

      //virtual Environment * environment();

   };


   CLASS_DECL_REMOTING_COMMON void defer_initialize_remoting();


   CLASS_DECL_REMOTING_COMMON int message_box(
      HWND hwnd,
      const ::scoped_string & scopedstrMessage,
      const ::scoped_string & scopedstrCaption,
      UINT uType);

} // remoting

