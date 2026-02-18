//
// Created by camilo on 2026-02-15 21:52 <3ThomasBorregaardSørensen!!
//

#pragma once

#include "acme/_operating_system.h"
#include "acme/platform/remoting.h"

namespace remoting
{


//   class Environment;


   class remoting :
   virtual public  ::platform::remoting
   {
   public:

      //::pointer < Environment > m_penvironment;


      remoting();
      ~remoting() override;

      //virtual Environment * environment();

   };


   void defer_initialize_remoting();


   int message_box(
      HWND hwnd,
      const ::scoped_string & scopedstrMessage,
      const ::scoped_string & scopedstrCaption,
      UINT uType);

} // remoting

