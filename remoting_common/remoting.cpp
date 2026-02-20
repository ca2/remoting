//
// Created by camilo on 2026-02-15 21:58 <3ThomasBorregaardSørensen!!
//
#include "framework.h"
#include "remoting.h"
//#include "remoting/remoting_common/win_system/Environment.h"


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
   //       øconstruct_new(m_penvironment);
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

         ::system()->m_premoting = ::system()->øcreate_new < ::remoting::remoting >();

      }

   }


   int message_box(
   HWND hwnd,
   const ::scoped_string & scopedstrMessage,
   const ::scoped_string & scopedstrCaption,
   UINT uType)
   {

      auto iResult = ::MessageBox(hwnd, ::wstring(scopedstrMessage), ::wstring(scopedstrCaption), uType);

      return iResult;

   }



} // namespace remoting



HINSTANCE remoting_impact_hinstance()
{

   return (HINSTANCE) ::system()->m_premoting->m_pHinstance;

}