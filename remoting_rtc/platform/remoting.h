//
// Created by camilo on 2026-02-15 21:52 <3ThomasBorregaardSørensen!!
// Changed 2026-04-29 12:20 by camilo <3ThomasBorregaardSørensen!! Mummi!! bilbo!!
//

#pragma once

//#include "acme/_operating_system.h"
#include "acme/platform/remoting.h"

namespace remoting
{


   //   class Environment;


   class CLASS_DECL_REMOTING remoting :
   virtual public  ::platform::remoting
   {
   public:

      //::pointer < Environment > m_penvironment;


      remoting();
      ~remoting() override;

      //virtual Environment * environment();

   };


   CLASS_DECL_REMOTING void defer_initialize_remoting();

} // remoting




namespace str
{

   CLASS_DECL_REMOTING ::string to_ansi(const ::scoped_string & scopedstr);

   CLASS_DECL_REMOTING ::string to_ansi(::u8 * puch, ::i32 iSize);

   CLASS_DECL_REMOTING bool checkAnsiConversion(const ::scoped_string & scopedstr);

}



