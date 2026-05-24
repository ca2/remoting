// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#pragma once


#include "remoting/remoting_rfb/region/Region.h"
//#include "remoting/remoting_rfb/region/::i32_point.h"


namespace remoting_rfb
{

   class CLASS_DECL_REMOTING_RDX UpdateContainer
   {
   public:
      UpdateContainer();
      ~UpdateContainer();

      UpdateContainer(const UpdateContainer &updatecontainer) { *this = updatecontainer; }
      UpdateContainer(UpdateContainer && updatecontainer);
      UpdateContainer &operator=(const UpdateContainer &src);
      UpdateContainer &operator=(UpdateContainer &&src);





      ::remoting_rfb::Region m_regionCopied;
      ::remoting_rfb::Region m_regionChanged;
      ::remoting_rfb::Region m_regionVideo;
      bool m_bScreenSizeChanged;
      bool m_bCursorPosChanged;
      bool m_bCursorShapeChanged;
      //::i32_point m_pointCopySource;
      //::i32_point m_pointCursorPos;
      ::i32_point m_pointCopySource;
      ::i32_point m_pointCursorPos;

      void clear();
      bool is_empty() const;

   };

   //// __UPDATECONTAINER_H__


} // namespace remoting_rfb





