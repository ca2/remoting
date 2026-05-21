// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "framework.h"
#include "CapContainer.h"
//#include aaa_<crtdbg.h>

namespace remoting
{
   CapContainer::CapContainer()
   {
   }

   CapContainer::~CapContainer()
   {
   }

   void CapContainer::addCap(::u32 code, const char *vendorSignature,
                             const char *nameSignature)
   {
      ASSERT(strlen(vendorSignature) == 4);
      ASSERT(strlen(nameSignature) == 8);
      Cap cap;
      cap.code = code;
      memcpy(cap.vendorSignature, vendorSignature, 4);
      memcpy(cap.nameSignature, nameSignature, 8);
      m_caps.add(cap);
   }

   ::u32 CapContainer::getCapCount() const
   {
      ::u32 capsSize = (::u32)m_caps.size();
      ASSERT(capsSize == m_caps.size());
      return capsSize;
   }

   void CapContainer::sendCaps(DataOutputStream *pdataoutputstream) const
   {
      for (CapVectorConstIter iter = m_caps.begin(); iter < m_caps.end(); iter++) {
         pdataoutputstream->writeUInt32((*iter).code);
         pdataoutputstream->write(&(*iter).vendorSignature, 4);
         pdataoutputstream->write(&(*iter).nameSignature, 8);
      }
   }

   bool CapContainer::includes(::u32 code) const
   {
      bool result = false;
      for (CapVectorConstIter iter = m_caps.begin(); iter < m_caps.end(); iter++) {
         if ((*iter).code == code) {
            result = true;
            break;
         }
      }
      return result;
   }
} // namespace remoting

