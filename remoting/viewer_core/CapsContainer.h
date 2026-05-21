// Copyright (C) 2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

//#include aaa_<map>
//#include aaa_<vector>

//#include "subsystem/thread/lockable_critical_section.h"
//////#include "subsystem/platform/::string.h"

#pragma once


namespace remoting_client
{
   class CLASS_DECL_REMOTING RfbCapabilityInfo {
   public:
      static const size_t vendorSigSize = 4;
      static const size_t nameSigSize = 8;

   public:
      ::u32 code;                            // numeric identifier
      char vendorSignature[vendorSigSize];    // vendor identification
      char nameSignature[nameSigSize];        // abbreviated option name

   public:
      //
      // Returns true if capability vendor and signature are equal to args
      //
      bool isEqual(const char *vendor, const char *signature) const;
   };

   // This class is thread-safe.
   class CapsContainer
   {
   public:
      CapsContainer();
      virtual ~CapsContainer();

      //
      // Add information about a particular capability into the object. There are
      // two functions to perform this task. These functions overwrite capability
      // records with the same code.
      //
      void add(const RfbCapabilityInfo *capinfo, const ::string desc = "");
      void add(::u32 code, const char *vendor, const char *name,
               ::string desc = "");

      //
      // Check if a capability with the specified code was added earlier.
      //
      bool isAdded(::u32 code) const;

      //
      // Fill in a rfbCapabilityInfo structure with contents corresponding to the
      // specified code. Returns true on success, false if the specified code is
      // not known.
      //
      bool getInfo(::u32 code, RfbCapabilityInfo *capinfo);

      //
      // Get a description string for the specified capability code. Returns NULL
      // either if the code is not known, or if there is no description for this
      // capability.
      //
      ::string getDescription(::u32 code) const;

      //
      // Mark the specified capability as "enabled". This function checks "vendor"
      // and "name" signatures in the existing record and in the argument structure
      // and enables the capability only if both records are the same.
      //
      bool enable(const RfbCapabilityInfo *capinfo);

      //
      // Check if the specified capability is known and enabled.
      //
      bool isEnabled(::u32 code) const;

      //
      // This function return count of enabled pcapabilitiesmanager.
      //
      size_t numEnabled() const;

      //
      // Return the capability code at the specified index.
      // List of pcapabilitiesmanager contained only enabled capability.
      // If the index is not valid, return 0.
      //
      ::u32 getByOrder(size_t idx);

      //
      // This method return ::list_base of enabled pcapabilitiesmanager.
      //
      void getEnabledCapabilities(::array_base<::u32> &codes) const;


   private:
      //
      // Check if a capability with the specified code was added earlier.
      // This function isn't thread-safe.
      //
      bool isKnown(::u32 code) const;

      ::map<::u32, RfbCapabilityInfo> infoMap;
      ::map<::u32, ::string> descMap;
      ::map<::u32, bool> enableMap;

      // List of enabled caps.
      ::array_base<::u32> m_plist;

      mutable lockable_critical_section m_mapLock;
   };
} // namespace remoting_client