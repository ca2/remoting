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
#include "framework.h"
#include "UpdateContainer.h"
namespace remoting_windows
{


   UpdateContainer::UpdateContainer() { clear(); }

   UpdateContainer::~UpdateContainer(void) {}

   void UpdateContainer::clear()
   {
      m_regionCopied.clear();
      m_regionChanged.clear();
      m_regionVideo.clear();
      m_bScreenSizeChanged = false;
      m_bCursorPosChanged = false;
      m_bCursorShapeChanged = false;
      m_pointCopySource.Null();
      m_pointCursorPos.Null();
   }

   UpdateContainer &UpdateContainer::operator=(const UpdateContainer &src)
   {
      m_regionCopied = src.m_regionCopied;
      m_regionChanged = src.m_regionChanged;
      m_regionVideo = src.m_regionVideo;
      m_bScreenSizeChanged = src.m_bScreenSizeChanged;
      m_bCursorPosChanged = src.m_bCursorPosChanged;
      m_bCursorShapeChanged = src.m_bCursorShapeChanged;
      m_pointCopySource = src.m_pointCopySource;
      m_pointCursorPos = src.m_pointCursorPos;

      return *this;
   }

   bool UpdateContainer::is_empty() const
   {
      return m_regionCopied.is_empty() && m_regionChanged.is_empty() && m_regionVideo.is_empty() && !m_bScreenSizeChanged &&
             !m_bCursorPosChanged && !m_bCursorShapeChanged;
   }


} // namespace remoting_windows
