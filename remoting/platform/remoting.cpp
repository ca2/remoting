//
//
//
// Created by camilo on 2026-02-15 21:58 <3ThomasBorregaardSørensen!!
// Changed by camilo on 2026-04-29 12:21 <3ThomasBorregaardSørensen!! Mummi!! bilbo!!
//
#include "framework.h"
#include "remoting.h"
#include "subsystem/node/OperatingSystem.h"


namespace remoting
{


   remoting::remoting()
   {


   }


   remoting::~remoting()
   {


   }


   // Environment * remoting::environment()
   // {
   //
   //    if (!m_penvironment)
   //    {
   //
   //       construct_newø(m_penvironment);
   //
   //    }
   //
   //    return m_penvironment;
   //
   // }


   void defer_initialize_remoting()
   {


      if (!::system()->m_premoting)
      {

         ::system()->m_premoting = ::system()->create_newø < ::remoting::remoting >();

      }

   }


} // namespace remoting


//
//HINSTANCE remoting_impact_hinstance()
//{
//
//   return (HINSTANCE) ::system()->m_premoting->m_pHinstance;
//
//}


namespace str
{


   ::string to_ansi(unsigned char * puch, int iSize)
   {

      ::string str;

      for (int i = 0; i < iSize; i++)
      {

         auto uch = puch[i];

         // easy/quick discard, no conversion
         if (!uch)
         {


         }
         else  if (uch & 0x80)
         {



         }
         else
         {

            str += (char) uch;

         }

      }

      return str;

   }


   ::string to_ansi(const ::scoped_string & scopedstr)
   {

      ::string str = to_ansi((unsigned char *) scopedstr.c_str(), scopedstr.length());

      return str;

   }


   bool checkAnsiConversion(const ::scoped_string & scopedstr)
   {

      auto strAnsi = to_ansi(scopedstr);

      return strAnsi == scopedstr;

   }

} // namespace str