// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "RfbKeySym.h"
#include "acme/constant/user_key.h"
#include "subsystem/_common_header.h"
#include "innate_subsystem/platform/subsystem.h"
#include "innate_subsystem/gui/KeyboardLayout.h"
#define XK_MISCELLANY
#include "remoting/remoting/rfb/keysymdef.h"
#include "acme/_operating_system.h"


namespace remoting
{
   RfbKeySym::RfbKeySym(RfbKeySymListener *extKeySymListener, ::subsystem::LogWriter * plogwriter)
   : m_extKeySymListener(extKeySymListener),
     m_plogwriter(plogwriter),
     m_winKeyIgnore(true)
   {
      m_keyboardstate.clearKeyState();
   }

   RfbKeySym::~RfbKeySym()
   {
   }

   void RfbKeySym::sendModifier(::user::enum_key ekeyModifier, bool down)
   {
      ::u32 rfbSym;
      bool success = m_keyMap.virtualCodeToKeySym(&rfbSym, ekeyModifier);
      ASSERT(success);
      sendKeySymEvent(rfbSym, down);

      m_keyboardstate.m_viewerKeyState[ekeyModifier] = down ? 128 : 0;
      ekeyModifier = distinguishLeftRightModifier(ekeyModifier, false);
      m_keyboardstate.m_serverKeyState[ekeyModifier] = down ? 128 : 0;
   }

   void RfbKeySym::processKeyHappening(::user::key_happening keyhappening)
   {
      m_plogwriter->debug("processKeyEvent() function called: virtKey = %#4.4x, bDown"
                 " = %#x, bExtended = %#", (::u32)keyhappening.m_euserkey, keyhappening.m_bDown,
                          keyhappening.m_bExtendedKey);

      auto euserkey = keyhappening.m_euserkey;
#ifdef WINDOWS
       // Ignoring win key (when fullscreen mode is off).
       if (m_winKeyIgnore) {
          if (euserkey == ::user::e_key_left_command || euserkey == ::user::e_key_right_command) { // Ignoring the Win key
             m_plogwriter->debug("Ignoring the Win key event");
             return;
          }
       }
#endif

      //bool down = (addKeyData & 0x80000000) == 0;
      bool down = keyhappening.m_bDown;
      m_plogwriter->debug("down = %u", (::u32)down);


//      bool ctrlPressed = m_keyboardstate.isPressed(VK_LCONTROL) ||
  //                       m_keyboardstate.isPressed(VK_RCONTROL);
      bool ctrlPressed = m_keyboardstate.isPressed(::user::e_key_left_control) ||
                         m_keyboardstate.isPressed(::user::e_key_right_control);
      //bool altPressed =
        // m_keyboardstate.isPressed(VK_LMENU) || m_keyboardstate.isPressed(VK_RMENU);
      bool altPressed =
         m_keyboardstate.isPressed(::user::e_key_left_alt) || m_keyboardstate.isPressed(::user::e_key_right_alt);
//      bool shiftPressed =
  //       m_keyboardstate.isPressed(VK_LSHIFT) || m_keyboardstate.isPressed(VK_RSHIFT);
      bool shiftPressed =
      m_keyboardstate.isPressed(::user::e_key_left_shift) || m_keyboardstate.isPressed(::user::e_key_right_shift);
      //bool capsToggled = m_keyboardstate.isPressed(VK_CAPITAL);
      bool capsToggled = m_keyboardstate.isPressed(::user::e_key_capslock);
      m_plogwriter->debug("ctrl = %u, alt = %u, shift = %u, caps toggled = %u",
        (::u32)ctrlPressed,
        (::u32)altPressed,
        (::u32)shiftPressed,
        (::u32)capsToggled);

      // Without distinguishing between left and right modifiers.
      m_keyboardstate.m_viewerKeyState[constrain(euserkey, ::user::e_key_none, ::user::e_key_count)] = down ? 128 : 0;
      m_keyboardstate.m_viewerKeyState[::user::e_key_capslock] = capsToggled ? 1 : 0;

      //bool extended = (addKeyData & 0x1000000) != 0; // 24 bit
      bool extended = keyhappening.m_bExtendedKey;
      euserkey = distinguishLeftRightModifier(euserkey, extended);

      // With distinguishing between left and right modifiers.
      m_keyboardstate.m_serverKeyState[constrain(euserkey, ::user::e_key_none, ::user::e_key_count)] = down ? 128 : 0;
      ::u32 rfbSym;
      if (m_keyMap.virtualCodeToKeySym(&rfbSym, constrain(euserkey, ::user::e_key_none, ::user::e_key_count))) {
         // Special case for VK_RETURN that have no self numpad code.
         // FIXME: May be replace this code to the virtualCodeToKeySym() function?
         if (rfbSym == XK_Return && extended) {
            rfbSym = XK_KP_Enter;
         }

         m_plogwriter->debug("The key has been mapped to the %#4.4x rfb symbol",
           rfbSym);
         sendKeySymEvent(rfbSym, down);
      } else {
         ::wstring chars;
         bool needReleaseModifiers = vkCodeToString(euserkey, down, &chars);
         if (ctrlPressed && altPressed && needReleaseModifiers) {
            m_plogwriter->debug("Release the ctrl and alt"
              " modifiers before send the key event(s)");
            releaseModifiers();
         }

         for (int i = 0; i < chars.size(); i++) {
            if (m_keyMap.unicodeCharToKeySym(chars[i], &rfbSym)) {
               sendKeySymEvent(rfbSym, down);
            } else {
               m_plogwriter->error("Can't translate the %#4.4x unicode character to an"
                 " rfb symbol to send it", (::u32)chars[i]);
            }
         }
         if (ctrlPressed && altPressed && needReleaseModifiers) {
            m_plogwriter->debug("Restore the ctrl and alt"
              " modifiers after send the key event(s)");
            restoreModifiers();
         }
      }
   }

   /*int RfbKeySym::GettingCharFromCtrlSymbol(int ch)
   {
      bool ctrlPressed = m_keyboardstate.isPressed(VK_LCONTROL) ||
                         m_keyboardstate.isPressed(VK_RCONTROL);
      bool altPressed =
         m_keyboardstate.isPressed(VK_LMENU) || m_keyboardstate.isPressed(VK_RMENU);
      bool shiftPressed =
         m_keyboardstate.isPressed(VK_LSHIFT) || m_keyboardstate.isPressed(VK_RSHIFT);
      ::u32 oldCh = (::u32)ch;
      if (ctrlPressed && !altPressed && ch < 32) {
         if (ch >= 1 && ch <= 26 && !shiftPressed) {
            ch += 96;
         }
         else {
            ch += 64;
         }
         m_plogwriter->debug("The %u char is a control symbol then"
           " it will be increased to %u",
           oldCh, (::u32)ch);
      }
      return ch;
   }*/

   // bool RfbKeySym::TryTranslateNotPrintableToUnicode(unsigned short virtKey, HKL currentLayout, WCHAR *unicodeChar)
   // {
   //    WCHAR outBuff[20];
   //    m_plogwriter->debug("Was get a not printable symbol then try get a printable"
   //      " with turned off the ctrl and alt modifiers");
   //    // E.g if pressed Ctrl + Alt + A
   //    // Try found char without modificators
   //    unsigned char withoutCtrlAltKbdState[256];
   //    memcpy(withoutCtrlAltKbdState, m_viewerKeyState, sizeof(withoutCtrlAltKbdState));
   //    withoutCtrlAltKbdState[VK_LCONTROL] = 0;
   //    withoutCtrlAltKbdState[VK_RCONTROL] = 0;
   //    withoutCtrlAltKbdState[VK_CONTROL] = 0;
   //    withoutCtrlAltKbdState[VK_LMENU] = 0;
   //    withoutCtrlAltKbdState[VK_RMENU] = 0;
   //    withoutCtrlAltKbdState[VK_MENU] = 0;
   //    int count = ToUnicodeEx(virtKey, 0, withoutCtrlAltKbdState, outBuff,
   //      sizeof(outBuff) / sizeof(WCHAR),
   //      0, currentLayout);
   //    if (count > 0) {
   //       outBuff[count] = 0;
   //    }
   //    else {
   //       outBuff[0] = 0;
   //    }
   //    m_plogwriter->debug("ToUnicodeEx() without ctrl and alt return {} wchars : {}", count, ::wstring(outBuff));
   //
   //    if (count == 1) { // other case will be ignored
   //       *unicodeChar = outBuff[0];
   //       return true;
   //    }
   //    return false;
   // }

   bool RfbKeySym::vkCodeToString(::user::enum_key ekey, bool down, ::wstring *res)
   {

      auto pkeyboardlayout = InnateSubsystem().keyboard_layout();

      return pkeyboardlayout->vkCodeToString(ekey, down, res, &m_keyboardstate);

      // HKL currentLayout = GetKeyboardLayout(0);
      //
      // int count = ToUnicodeEx(virtKey, 0, m_viewerKeyState, outBuff,
      //     sizeof(outBuff) / sizeof(WCHAR),
      //     0, currentLayout);
      //
      // // For keyboards with dead keys
      // if (m_doubleDeadCatched && !down && !m_allowProcessDoubleChar) {
      //    m_plogwriter->debug("Disable process char event");
      //    m_allowProcessCharEvent = false;
      //    m_doubleDeadCatched = false;
      // }
      //
      // // For keyboards with dead keys
      // if (count == 2 && (isDoubleDeadCharacters(outBuff))) {
      //    m_plogwriter->debug("Extra double dead key catched.");
      //    if (down) {
      //       m_plogwriter->debug("Enable process char event. Enable process double char.");
      //       m_allowProcessDoubleChar = true;
      //       m_allowProcessCharEvent = true;
      //       m_doubleDeadCatched = true;
      //       count = ToUnicodeEx(virtKey, 0, m_viewerKeyState, outBuff,
      //           sizeof(outBuff) / sizeof(WCHAR),
      //           0, currentLayout);
      //       if (count > 0) {
      //          outBuff[count] = 0;
      //       } else {
      //          outBuff[0] = 0;
      //       }
      //       //ToUnicodeEx can return without null termination
      //       m_plogwriter->debug("ToUnicodeEx() function called second. Returned: {}. Output buff: {}", count, ::wstring(outBuff));
      //       res->clear();
      //       return false;
      //    } else if (!m_allowProcessCharEvent) {
      //       m_plogwriter->debug("Disable second calling ToUnicodeEx()");
      //       res->clear();
      //       return false;
      //    }
      // }
      //
      // if (count > 0) {
      //    count = ToUnicodeEx(virtKey, 0, m_viewerKeyState, outBuff,
      //          sizeof(outBuff) / sizeof(WCHAR), 0, currentLayout);
      // }
      // if (count > 0) {
      //    outBuff[count] = 0;
      // } else {
      //    outBuff[0] = 0;
      // }
      // m_plogwriter->debug("ToUnicodeEx() returned {}, output buff : {}", count, ::wstring(outBuff));
      //
      // if (count == 1 && !m_allowProcessCharEvent) {
      //    res->add(GettingCharFromCtrlSymbol(outBuff[0]));
      //    needReleaseModifiers = true;
      // }
      // // Two or more (only two of them is relevant?)
      // else if (count > 1) {
      //    for (int i = 0; i < count; i++) {
      //       res->add(GettingCharFromCtrlSymbol(outBuff[i]));
      //    }
      //    needReleaseModifiers = true;
      // } else if (count == 0) {
      //    WCHAR unicodeChar;
      //    if (TryTranslateNotPrintableToUnicode(virtKey, currentLayout, &unicodeChar)) {
      //       res->add(unicodeChar);
      //    }
      // } else if (count == -1 && down) {
      //    m_plogwriter->debug("Dead key pressed, waitThreadToFinish for a char event");
      //    m_allowProcessCharEvent = true;
      // }
      //return needReleaseModifiers;
   }

   void RfbKeySym::processCharEvent(int charCode,
                                    ::u32 addKeyData)
   {
      if (m_keyboardstate.m_allowProcessCharEvent) {
         m_plogwriter->debug("processCharEvent() function called with alowed processing:"
                    " charCode = %#4.4x, addKeyData = %#x",
                    (::u32)charCode, addKeyData);
         // For keyboards with dead keys
         if (m_keyboardstate.m_allowProcessDoubleChar)
         {
            m_keyboardstate.m_allowProcessDoubleChar = false;
            bool ctrlPressed = m_keyboardstate.isPressed(::user::e_key_left_control) || m_keyboardstate.isPressed(::user::e_key_right_control);
            bool altPressed = m_keyboardstate.isPressed(::user::e_key_left_alt) || m_keyboardstate.isPressed(::user::e_key_right_alt);
            if (ctrlPressed && altPressed) {
               m_plogwriter->debug("Release the ctrl and alt"
                 " modifiers before send dead combination");
               releaseModifiers();
            }
         } else {
            m_keyboardstate.m_allowProcessCharEvent = false;
         }

         ::u32 rfbSym;
         if (m_keyMap.unicodeCharToKeySym(charCode, &rfbSym)) {
            sendKeySymEvent(rfbSym, true);
            sendKeySymEvent(rfbSym, false);
         } else {
            m_plogwriter->error("Can't translate the %#4.4x unicode character to an"
                       " rfb symbol to send it", (::u32)charCode);
         }
      }
   }

   void RfbKeySym::processFocusRestoration()
   {
      m_plogwriter->information("Process focus restoration in the RfbKeySym class");

      // // Send a modifier key state based on physical keyboard state, not thread key scopedstrMessage queue.
      // unsigned char keys[9] = {VK_CONTROL, VK_RCONTROL, VK_LCONTROL,
      //                          VK_MENU, VK_RMENU, VK_LMENU,
      //                          VK_SHIFT, VK_RSHIFT, VK_LSHIFT};
      // for (int i = 0; i < 9 ; i++) {
      //    unsigned char key = keys[i];
      //    unsigned char state = GetAsyncKeyState(key) >> 8;
      //    checkAndSendDiff(key, state);
      // }
   }

   void RfbKeySym::processFocusLoss()
   {
      m_plogwriter->information("Process focus loss in the RfbKeySym class");

      // Send a modifier key up only if the key is down.
      checkAndSendDiff(::user::e_key_control, 0);
      checkAndSendDiff(::user::e_key_right_control, 0);
      checkAndSendDiff(::user::e_key_left_control, 0);
      checkAndSendDiff(::user::e_key_alt, 0);
      checkAndSendDiff(::user::e_key_left_alt, 0);
      checkAndSendDiff(::user::e_key_right_alt, 0);
      checkAndSendDiff(::user::e_key_shift, 0);
      checkAndSendDiff(::user::e_key_left_shift, 0);
      checkAndSendDiff(::user::e_key_right_shift, 0);
      checkAndSendDiff(::user::e_key_left_command, 0);
      checkAndSendDiff(::user::e_key_right_command, 0);

   }

   void RfbKeySym::sendCtrlAltDel()
   {
      releaseModifier(::user::e_key_right_command);
      releaseModifier(::user::e_key_left_command);
      releaseModifier(::user::e_key_left_shift);
      releaseModifier(::user::e_key_right_shift);
      releaseMeta();

      sendVerbatimKeySymEvent(XK_Control_L, true);
      sendVerbatimKeySymEvent(XK_Alt_L, true);
      sendVerbatimKeySymEvent(XK_Delete, true);
      sendVerbatimKeySymEvent(XK_Delete, false);
      sendVerbatimKeySymEvent(XK_Alt_L, false);
      sendVerbatimKeySymEvent(XK_Control_L, false);

      restoreMeta();
      restoreModifier(::user::e_key_right_shift);
      restoreModifier(::user::e_key_left_shift);
      restoreModifier(::user::e_key_left_command);
      restoreModifier(::user::e_key_right_command);
   }

   void RfbKeySym::setWinKeyIgnore(bool winKeyIgnore)
   {
      m_winKeyIgnore = winKeyIgnore;
   }

   void RfbKeySym::releaseModifiers()
   {
      releaseModifier(::user::e_key_left_control);
      releaseModifier(::user::e_key_right_control);
      releaseModifier(::user::e_key_left_alt);
      releaseModifier(::user::e_key_right_alt);
   }

   void RfbKeySym::restoreModifiers()
   {
      restoreModifier(::user::e_key_left_control);
      restoreModifier(::user::e_key_right_control);
      restoreModifier(::user::e_key_left_alt);
      restoreModifier(::user::e_key_right_alt);
   }

   void RfbKeySym::releaseModifier(::user::enum_key ekeyModifier)
   {
      ::u32 rfbSym;
      if (m_keyboardstate.isPressed(ekeyModifier)) {
         bool success = m_keyMap.virtualCodeToKeySym(&rfbSym, ekeyModifier);
         ASSERT(success);
         sendKeySymEvent(rfbSym, false);
      }
   }

   void RfbKeySym::releaseMeta()
   {
      if (m_keyboardstate.m_leftMetaIsPressed)
      {
         sendKeySymEvent(XK_Meta_L, false);
      }
      if (m_keyboardstate.m_rightMetaIsPressed)
      {
         sendKeySymEvent(XK_Meta_R, false);
      }
   }

   void RfbKeySym::restoreMeta()
   {
      if (m_keyboardstate.m_leftMetaIsPressed)
      {
         sendKeySymEvent(XK_Meta_L, true);
      }
      if (m_keyboardstate.m_rightMetaIsPressed)
      {
         sendKeySymEvent(XK_Meta_R, true);
      }
   }

   void RfbKeySym::restoreModifier(::user::enum_key ekeyModifier)
   {
      ::u32 rfbSym;
      if (m_keyboardstate.isPressed(ekeyModifier))
      {
         bool success = m_keyMap.virtualCodeToKeySym(&rfbSym, ekeyModifier);
         ASSERT(success);
         sendKeySymEvent(rfbSym, true);
      }
   }

   void RfbKeySym::checkAndSendDiff(::user::enum_key ekey, unsigned char state)
   {
      bool testedState = (state & 128) != 0;
      m_keyboardstate.m_viewerKeyState[ekey] = testedState ? 128 : 0;
      ekey = distinguishLeftRightModifier(ekey, false);

      bool srvState = (m_keyboardstate.m_serverKeyState[ekey] & 128) != 0;
      m_keyboardstate.m_serverKeyState[ekey] = testedState ? 128 : 0;

      if (testedState != srvState) {
         ::u32 rfbSym;
         bool success = m_keyMap.virtualCodeToKeySym(&rfbSym, ekey);
         ASSERT(success);
         sendKeySymEvent(rfbSym, testedState);
      }
   }


   void RfbKeySym::sendKeySymEvent(::u32 rfbKeySym, bool down)
   {
      // // Translate Alt to Meta if Scroll Lock is on.
      // if (rfbKeySym == XK_Alt_L || rfbKeySym == XK_Alt_R) {
      //    bool scrollLockOn = (GetKeyState(VK_SCROLL) & 1) != 0;
      //    if (scrollLockOn) {
      //       if (rfbKeySym == XK_Alt_L) {
      //          rfbKeySym = XK_Meta_L;
      //          m_leftMetaIsPressed = down;
      //       } else { // assuming (rfbKeySym == XK_Alt_R)
      //          rfbKeySym = XK_Meta_R;
      //          m_rightMetaIsPressed = down;
      //       }
      //    }
      // }

      // Send the keyboard event.
      sendVerbatimKeySymEvent(rfbKeySym, down);
   }

   void RfbKeySym::sendVerbatimKeySymEvent(::u32 rfbKeySym, bool down)
   {
      m_extKeySymListener->onRfbKeySymEvent(rfbKeySym, down);
   }

::user::enum_key RfbKeySym::distinguishLeftRightModifier(::user::enum_key ekeyModifier,
                                                         bool isRightHint)
   {
      ::user::enum_key ekey = ekeyModifier;
      if (ekeyModifier == ::user::e_key_control) {
         ekey = isRightHint ? ::user::e_key_right_control : ::user::e_key_left_control;
      }
      if (ekeyModifier == ::user::e_key_alt) {
         ekey = isRightHint ? ::user::e_key_right_alt : ::user::e_key_left_alt;
      }
      if (ekeyModifier == ::user::e_key_shift) {
         ekey = isRightHint ? ::user::e_key_right_shift : ::user::e_key_left_shift;
      }
      return ekey;
   }
} // namespace remoting
