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



//#include "remoting/remoting_rfb/region/::i32_point.h"
#include "subsystem/_common_header.h"
#include "remoting/remoting_rfb/desktop/CopyRectDetector.h"
#include "acme/prototype/geometry2d/rectangle.h"

#include <CoreGraphics/CoreGraphics.h>


namespace remoting_macos
{

struct WinProp
{
    ::u32        windowId;
    CGRect          rect;

    WinProp()
        : windowId(0)
    {
    }

    WinProp(uint32_t id, const CGRect& r)
        : windowId(id),
          rect(r)
    {
    }
};

class CopyRectDetector :
virtual public ::remoting_rfb::CopyRectDetector
{
public:
   
//private:
    ::array_base<WinProp> m_lastWinProps;
    ::array_base<WinProp> m_newWinProps;

    CGRect  m_rectCopy;
    CGPoint m_pointSource;
   
    CopyRectDetector();
    ~CopyRectDetector();

   void detectWindowMovements(::i32_rectangle &rectangleCopy, ::i32_point & pointSource) override;

    void _detectWindowMovements(CGRect& rectCopy,
                               CGPoint& pointSource);

//private:
    bool checkWindowMovements(CFDictionaryRef windowInfo);

    bool findPrevWinProps(uint32_t windowId,
                          CGRect& rect);


};


} // namespace remoting_macos
 


