// Created by camilo on 2026-03-08 04:09 <3ThomasBorregaardSørensen!!
#include "framework.h"
#include "keyboard_layout_change.h"
//#include "acme/_operating_system.h"


namespace remoting_remoting
{


   keyboard_layout_change::keyboard_layout_change()
   {

   }


   keyboard_layout_change::~keyboard_layout_change()
   {

   }


   void keyboard_layout_change::on_websocket_text(const ::scoped_string& scopedstrText)
   {

      ::payload payload;

      payload.parse_network_payload(scopedstrText);

      auto& set = payload.property_set_reference();

      auto strNotification = set["notification"];

      if (strNotification == "keyboard_layout_change")
      {

         ::string strKeyboardLayoutId = set["keyboard_layout_id"];

         HKL hkl = nullptr;

         if (strKeyboardLayoutId.case_insensitive_equals("com.apple.keylayout.Danish"))
         {

            information("remote device changed keyboard layout to Danish");

            // Load Danish keyboard layout
            hkl = LoadKeyboardLayoutA("00000406", KLF_ACTIVATE);

            //if (!hkl) 
            //{
            //   
            //   warning("Failed to load Danish keyboard layout");
            //   
            //   return;

            //}

            //// Activate it
            //ActivateKeyboardLayout(hkl, 0);

            //information("Switched to Danish keyboard layout.");

         }
         else if (strKeyboardLayoutId.case_insensitive_equals("com.apple.keylayout.Brazilian-ABNT2"))
         {

            information("remote device changed keyboard layout to ABNT2");

            // Load Danish keyboard layout
            hkl = LoadKeyboardLayoutA("00010416", KLF_ACTIVATE);

            //if (!hkl)
            //{

            //   warning("Failed to load ABNT2 keyboard layout");

            //   return;

            //}

            //// Activate it
            ////ActivateKeyboardLayout(hkl, 0);

            

            //information("Switched to ABNT2 keyboard layout.");

         }

         if (hkl)
         {

            ::PostMessage(HWND_BROADCAST, WM_INPUTLANGCHANGEREQUEST, 0, (::lparam)hkl);

         }

      }


   }


} // namespace remoting_remoting



