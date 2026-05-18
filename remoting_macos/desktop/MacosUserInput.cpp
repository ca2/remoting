// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
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
#include "remoting/remoting_macos/desktop/MacosUserInput.h"
#include "subsystem/thread/DesktopSelector.h"
#include "subsystem/node/OperatingSystem.h"
#include "subsystem/node/Keyboard.h"
#include "innate_subsystem/gui/WindowFinder.h"
#include "subsystem/node/Clipboard2.h"
#include "subsystem/platform/BrokenHandleException.h"
#include "acme/operating_system/macos/os.h"
#include <ApplicationServices/ApplicationServices.h>
#include <cmath>

namespace remoting_macos
{

   namespace
   {

      ::i32_rectangle cg_display_rect_to_remoting_rect(const CGRect & rect, const ::i32_rectangle & rectangleVirtual)
      {
         ::i32_rectangle rectangle;
         rectangle.left = (int) std::lround(rect.origin.x) - rectangleVirtual.left;
         rectangle.top = (int) std::lround(rect.origin.y) - rectangleVirtual.top;
         rectangle.right = rectangle.left + (int) std::lround(rect.size.width);
         rectangle.bottom = rectangle.top + (int) std::lround(rect.size.height);
         return rectangle;
      }

      bool get_active_displays(CGDirectDisplayID * displays, uint32_t & count)
      {
         count = 64;
         return CGGetActiveDisplayList(count, displays, &count) == kCGErrorSuccess && count > 0;
      }

      ::i32_rectangle get_virtual_display_bounds()
      {
         ::i32_rectangle rectangle;
         rectangle.clear();

         CGDirectDisplayID displays[64] = {};
         uint32_t count = 0;
         if (!get_active_displays(displays, count))
         {
            return rectangle;
         }

         auto first = true;
         for (uint32_t i = 0; i < count; ++i)
         {
            const auto rect = CGDisplayBounds(displays[i]);
            ::i32_rectangle displayRectangle;
            displayRectangle.left = (int) std::lround(rect.origin.x);
            displayRectangle.top = (int) std::lround(rect.origin.y);
            displayRectangle.right = displayRectangle.left + (int) std::lround(rect.size.width);
            displayRectangle.bottom = displayRectangle.top + (int) std::lround(rect.size.height);

            if (first)
            {
               rectangle = displayRectangle;
               first = false;
            }
            else
            {
               rectangle.unite(displayRectangle);
            }
         }

         return rectangle;
      }

      CGPoint to_cg_point(const ::i32_point & point)
      {
         const auto rectangleVirtual = get_virtual_display_bounds();
         return CGPointMake(rectangleVirtual.left + point.x, rectangleVirtual.top + point.y);
      }

      void post_mouse_event(CGEventType type, CGPoint point, CGMouseButton button)
      {
         auto event = CGEventCreateMouseEvent(nullptr, type, point, button);
         if (event)
         {
            CGEventPost(kCGHIDEventTap, event);
            CFRelease(event);
         }
      }

      void post_scroll_event(int delta)
      {
         auto event = CGEventCreateScrollWheelEvent(nullptr, kCGScrollEventUnitLine, 1, delta);
         if (event)
         {
            CGEventPost(kCGHIDEventTap, event);
            CFRelease(event);
         }
      }

      bool cg_window_rect(CGWindowID window, ::i32_rectangle & rectangle)
      {
         auto windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionIncludingWindow, window);
         if (!windowList || CFArrayGetCount(windowList) == 0)
         {
            if (windowList)
            {
               CFRelease(windowList);
            }
            return false;
         }

         auto dictionary = (CFDictionaryRef) CFArrayGetValueAtIndex(windowList, 0);
         auto bounds = (CFDictionaryRef) CFDictionaryGetValue(dictionary, kCGWindowBounds);
         CGRect rect = {};
         auto ok = bounds && CGRectMakeWithDictionaryRepresentation(bounds, &rect);
         CFRelease(windowList);

         if (!ok)
         {
            return false;
         }

         rectangle.left = (int) std::lround(rect.origin.x);
         rectangle.top = (int) std::lround(rect.origin.y);
         rectangle.right = rectangle.left + (int) std::lround(rect.size.width);
         rectangle.bottom = rectangle.top + (int) std::lround(rect.size.height);
         return true;
      }

      bool cg_window_pid(CFDictionaryRef dictionary, ::process_identifier & processidentifier)
      {
         auto number = (CFNumberRef) CFDictionaryGetValue(dictionary, kCGWindowOwnerPID);
         pid_t pid = 0;
         if (!number || !CFNumberGetValue(number, kCFNumberIntType, &pid))
         {
            return false;
         }
         processidentifier = pid;
         return true;
      }

      bool cg_window_name_contains(CFDictionaryRef dictionary, const ::scoped_string & windowName)
      {
         auto name = (CFStringRef) CFDictionaryGetValue(dictionary, kCGWindowName);
         if (!name)
         {
            name = (CFStringRef) CFDictionaryGetValue(dictionary, kCGWindowOwnerName);
         }
         if (!name)
         {
            return false;
         }

         char buffer[1024] = {};
         if (!CFStringGetCString(name, buffer, sizeof(buffer), kCFStringEncodingUTF8))
         {
            return false;
         }

         ::string strName(buffer);
         return ::found(strName.case_insensitive_find(windowName));
      }

   } // namespace


   MacosUserInput::MacosUserInput() :
   m_prevKeyFlag(0)
   {
   }



   MacosUserInput::~MacosUserInput(void) {  }

   void MacosUserInput::initialize_user_input(::subsystem::ClipboardListener *pclipboardlistener, bool ctrlAltDelEnabled,
                 ::subsystem::LogWriter * plogwriter)
   {
      m_prevKeyFlag = 0;
      m_inputInjector.initialize_input_injector(ctrlAltDelEnabled, plogwriter);
      m_plogwriter = plogwriter;
      constructø(m_pclipboard);
      m_pclipboard->initialize_clipboard2(pclipboardlistener, plogwriter);
   }


   void MacosUserInput::setMouseEvent(const ::i32_point pointNewPosition, unsigned char keyFlag)
   {
      m_plogwriter->debug("setMouseEvent ({},{}):{}", pointNewPosition.x, pointNewPosition.x, keyFlag);
      const auto point = to_cg_point(pointNewPosition);

      auto dragType = kCGEventMouseMoved;
      if ((keyFlag & 1) != 0)
      {
         dragType = kCGEventLeftMouseDragged;
      }
      else if ((keyFlag & 4) != 0)
      {
         dragType = kCGEventRightMouseDragged;
      }
      else if ((keyFlag & 2) != 0)
      {
         dragType = kCGEventOtherMouseDragged;
      }
      post_mouse_event(dragType, point, kCGMouseButtonLeft);

      auto prevState = (m_prevKeyFlag & 1) != 0;
      auto currState = (keyFlag & 1) != 0;
      if (currState != prevState)
      {
         post_mouse_event(currState ? kCGEventLeftMouseDown : kCGEventLeftMouseUp, point, kCGMouseButtonLeft);
      }
      prevState = (m_prevKeyFlag & 2) != 0;
      currState = (keyFlag & 2) != 0;
      if (currState != prevState)
      {
         post_mouse_event(currState ? kCGEventOtherMouseDown : kCGEventOtherMouseUp, point, kCGMouseButtonCenter);
      }
      prevState = (m_prevKeyFlag & 4) != 0;
      currState = (keyFlag & 4) != 0;
      if (currState != prevState)
      {
         post_mouse_event(currState ? kCGEventRightMouseDown : kCGEventRightMouseUp, point, kCGMouseButtonRight);
      }

      bool prevWheelUp = (m_prevKeyFlag & 8) != 0;
      bool currWheelUp = (keyFlag & 8) != 0;
      bool prevWheelDown = (m_prevKeyFlag & 16) != 0;
      bool currWheelDown = (keyFlag & 16) != 0;

      if (currWheelUp && !prevWheelUp)
      {
         post_scroll_event(1);
      }
      else if (currWheelDown && !prevWheelDown)
      {
         post_scroll_event(-1);
      }

      m_prevKeyFlag = keyFlag;
   }

   void MacosUserInput::setNewClipboard(const ::scoped_string &newClipboard)
   {
      m_pclipboard->writeToClipBoard(newClipboard);
   }

   void MacosUserInput::setKeyboardEvent(::u32 keySym, bool down)
   {
      try
      {
         m_plogwriter->information("Received the %#4.4x keysym, down = {}", keySym, (int)down);
         ::user::enum_key euserkey;
         int ch;
         bool release = !down;
         bool extended;

         if (m_keyMap.keySymToVirtualCode(keySym, &euserkey, &extended))
         {
            m_inputInjector.injectKeyEvent(euserkey, release, extended);
         }
         else if (m_keyMap.keySymToUnicodeChar(keySym, &ch))
         {
            m_inputInjector.injectCharEvent(ch, release);
         }
         else
         {
            ::string scopedstrMessage;
            scopedstrMessage.format("Unknown {} keysym", keySym);
            throw ::subsystem::Exception(scopedstrMessage);
         }
      }
      catch (::subsystem::Exception &someEx)
      {
         m_plogwriter->error("::subsystem::Exception while processing key event: {}", someEx.get_message());
      }
   }

   void MacosUserInput::getCurrentUserInfo(::string &desktopName, ::string &userName)
   {
      if (!MainSubsystem().DesktopSelector().getCurrentDesktopName(desktopName) &&
          !MainSubsystem().OperatingSystem().getCurrentUserName(userName, m_plogwriter))
      {
         ::string errMess;
         errMess  = MainSubsystem().OperatingSystem().getErrStr("Can't get current user info");
         throw ::subsystem::Exception(errMess);
      }
   }

   void MacosUserInput::getPrimaryDisplayCoords(::i32_rectangle & rectangle)
   {
      const auto rectangleVirtual = get_virtual_display_bounds();
      rectangle = cg_display_rect_to_remoting_rect(CGDisplayBounds(CGMainDisplayID()), rectangleVirtual);
   }

   void MacosUserInput::getDisplayNumberCoords(::i32_rectangle & rectangle, unsigned char dispNumber)
   {
      rectangle.clear();
      CGDirectDisplayID displays[64] = {};
      uint32_t count = 0;
      if (!get_active_displays(displays, count) || dispNumber == 0 || dispNumber > count)
      {
         return;
      }

      const auto rectangleVirtual = get_virtual_display_bounds();
      rectangle = cg_display_rect_to_remoting_rect(CGDisplayBounds(displays[dispNumber - 1]), rectangleVirtual);
   }

   ::int_rectangle_array_base MacosUserInput::getDisplaysCoords()
   {
      ::int_rectangle_array_base rectangles;
      CGDirectDisplayID displays[64] = {};
      uint32_t count = 0;
      if (!get_active_displays(displays, count))
      {
         return rectangles;
      }

      const auto rectangleVirtual = get_virtual_display_bounds();
      for (uint32_t i = 0; i < count; ++i)
      {
         rectangles.add(cg_display_rect_to_remoting_rect(CGDisplayBounds(displays[i]), rectangleVirtual));
      }
      return rectangles;
   }

   void MacosUserInput::getNormalizedRect(::i32_rectangle & rectangle) { toFbCoordinates(rectangle); }

   void MacosUserInput::toFbCoordinates(::i32_rectangle & rectangle)
   {
      const auto rectangleVirtual = get_virtual_display_bounds();
      rectangle.offset(-rectangleVirtual.left, -rectangleVirtual.top);
   }

   void MacosUserInput::getWindowCoords(const ::operating_system::window & operatingsystemwindow, ::i32_rectangle & rectangle)
   {
      rectangle.clear();
      if (cg_window_rect((CGWindowID) as_u64(operatingsystemwindow), rectangle))
      {
         toFbCoordinates(rectangle);
      }
      else
      {
         ::string errMess;
         errMess = MainSubsystem().OperatingSystem().getErrStr("Can't get window coordinates");
         throw ::subsystem::BrokenHandleException(errMess);
      }
   }

   operating_system::window MacosUserInput::getWindowHandleByName(const ::scoped_string &windowName)
   {
      auto windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
      if (!windowList)
      {
         return {};
      }

      const auto count = CFArrayGetCount(windowList);
      for (CFIndex i = 0; i < count; ++i)
      {
         auto dictionary = (CFDictionaryRef) CFArrayGetValueAtIndex(windowList, i);
         if (!cg_window_name_contains(dictionary, windowName))
         {
            continue;
         }

         auto number = (CFNumberRef) CFDictionaryGetValue(dictionary, kCGWindowNumber);
         CGWindowID window = 0;
         if (number && CFNumberGetValue(number, kCFNumberSInt32Type, &window))
         {
            CFRelease(windowList);
            return as_operating_system_window(window);
         }
      }

      CFRelease(windowList);
      return {};
   }

   void MacosUserInput::getApplicationRegion(const ::process_identifier & processidentifier, ::remoting::Region & region)
   {
      region.clear();
      ::remoting::Region occludedRegion;
      auto windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
      if (!windowList)
      {
         return;
      }

      const auto count = CFArrayGetCount(windowList);
      for (CFIndex i = 0; i < count; ++i)
      {
         auto dictionary = (CFDictionaryRef) CFArrayGetValueAtIndex(windowList, i);
         auto layer = (CFNumberRef) CFDictionaryGetValue(dictionary, kCGWindowLayer);
         int layerValue = 0;
         if (layer && CFNumberGetValue(layer, kCFNumberIntType, &layerValue) && layerValue != 0)
         {
            continue;
         }

         ::process_identifier windowProcessIdentifier;
         if (!cg_window_pid(dictionary, windowProcessIdentifier))
         {
            continue;
         }

         auto bounds = (CFDictionaryRef) CFDictionaryGetValue(dictionary, kCGWindowBounds);
         CGRect rect = {};
         if (!bounds || !CGRectMakeWithDictionaryRepresentation(bounds, &rect))
         {
            continue;
         }

         ::i32_rectangle rectangle;
         rectangle.left = (int) std::lround(rect.origin.x);
         rectangle.top = (int) std::lround(rect.origin.y);
         rectangle.right = rectangle.left + (int) std::lround(rect.size.width);
         rectangle.bottom = rectangle.top + (int) std::lround(rect.size.height);

         if (windowProcessIdentifier == processidentifier)
         {
            ::remoting::Region windowRegion(rectangle);
            windowRegion.subtract(occludedRegion);
            region.add(windowRegion);
         }
         else
         {
            occludedRegion.addRect(rectangle);
         }
      }

      CFRelease(windowList);
      const auto rectangleVirtual = get_virtual_display_bounds();
      region.translate(-rectangleVirtual.left, -rectangleVirtual.top);
   }

   bool MacosUserInput::isApplicationInFocus(const ::process_identifier & processidentifier)
   {
      auto windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
      if (!windowList)
      {
         return false;
      }

      const auto count = CFArrayGetCount(windowList);
      for (CFIndex i = 0; i < count; ++i)
      {
         auto dictionary = (CFDictionaryRef) CFArrayGetValueAtIndex(windowList, i);
         auto layer = (CFNumberRef) CFDictionaryGetValue(dictionary, kCGWindowLayer);
         int layerValue = 0;
         if (layer && CFNumberGetValue(layer, kCFNumberIntType, &layerValue) && layerValue != 0)
         {
            continue;
         }

         ::process_identifier foregroundProcessIdentifier;
         const auto ok = cg_window_pid(dictionary, foregroundProcessIdentifier);
         CFRelease(windowList);
         return ok && foregroundProcessIdentifier == processidentifier;
      }

      CFRelease(windowList);
      return false;
   }


} // namespace remoting_macos
