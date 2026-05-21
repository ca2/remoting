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
#include "CopyRectDetector.h"
#include "subsystem/_common_header.h"
#include "acme/_operating_system.h"
#include "acme/operating_system/apple/_apple.h"



namespace remoting_macos
{

CopyRectDetector::CopyRectDetector()
{
    m_rectCopy = CGRectZero;
    m_pointSource = CGPointZero;
}

CopyRectDetector::~CopyRectDetector()
{
}


void CopyRectDetector::detectWindowMovements(::i32_rectangle &rectangleCopy, ::i32_point & pointSource)
{
   
   CGRect cgrectCopy{};
   
   CGPoint cgpointSource{};
   
   _detectWindowMovements(cgrectCopy , cgpointSource);
   
   
   screen_coordinates_aware_copy(rectangleCopy, cgrectCopy);
   
   screen_coordinates_aware_copy(pointSource, cgpointSource);
   
   
}


void CopyRectDetector::_detectWindowMovements(
    CGRect& rectCopy,
    CGPoint& pointSource)
{
    m_rectCopy = CGRectZero;
    m_pointSource = CGPointZero;

    CFArrayRef windowList =
        CGWindowListCopyWindowInfo(
            kCGWindowListOptionOnScreenOnly,
            kCGNullWindowID);

    if (windowList)
    {
        CFIndex count = CFArrayGetCount(windowList);

        for (CFIndex i = 0; i < count; ++i)
        {
            CFDictionaryRef windowInfo =
                (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);

            checkWindowMovements(windowInfo);
        }

        CFRelease(windowList);
    }

    m_lastWinProps = m_newWinProps;
    m_newWinProps.clear();

    rectCopy = m_rectCopy;
    pointSource = m_pointSource;
}

bool CopyRectDetector::checkWindowMovements(
    CFDictionaryRef windowInfo)
{
    // Window ID
    CFNumberRef windowIdNumber =
        (CFNumberRef)CFDictionaryGetValue(
            windowInfo,
            kCGWindowNumber);

    if (!windowIdNumber)
        return true;

    int windowId = 0;

    CFNumberGetValue(
        windowIdNumber,
        kCFNumberIntType,
        &windowId);

    // Bounds
    CFDictionaryRef boundsDict =
        (CFDictionaryRef)CFDictionaryGetValue(
            windowInfo,
            kCGWindowBounds);

    if (!boundsDict)
        return true;

    CGRect currentRect;

    if (!CGRectMakeWithDictionaryRepresentation(
            boundsDict,
            &currentRect))
    {
        return true;
    }

    // Ignore tiny windows
    if (currentRect.size.width <= 1 ||
        currentRect.size.height <= 1)
    {
        return true;
    }

    // Save current properties
   m_newWinProps.add({(::u32)windowId, currentRect});

    CGRect oldRect;

    if (findPrevWinProps(windowId, oldRect))
    {
        CGPoint oldPos = oldRect.origin;
        CGPoint newPos = currentRect.origin;

        bool moved =
            oldPos.x != newPos.x ||
            oldPos.y != newPos.y;

        double oldArea =
            oldRect.size.width *
            oldRect.size.height;

        double newArea =
            currentRect.size.width *
            currentRect.size.height;

        double currentBestArea =
            m_rectCopy.size.width *
            m_rectCopy.size.height;

        if (moved && newArea > currentBestArea)
        {
            m_rectCopy = currentRect;
            m_pointSource = oldPos;
        }
    }

    return true;
}

bool CopyRectDetector::findPrevWinProps(
    uint32_t windowId,
    CGRect& rect)
{
    for (const auto& prop : m_lastWinProps)
    {
        if (prop.windowId == windowId)
        {
            rect = prop.rect;
            return true;
        }
    }

    return false;
}


} // namespace remoting_macos




