//
// Created by camilo on 2026-05-23 10:30 <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
//
#pragma once
// ============================================================================
// Basic COM RefCount Helper
// ============================================================================

namespace windows
{


   class com_base :
      virtual public ::particle
   {
   public:


      com_base() {}
      ~com_base() override {}


      ULONG InternalAddRef()
      {

         return this->increment_reference_count();

      }


      ULONG InternalRelease()
      {

         return this->release();

      }


   };


} // namespace windows

