// Created by camilo on 2026-03-08 04:09 <3ThomasBorregaardSørensen!!
#include "framework.h"
#include "keyboard_layout_change.h"


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

         if (strKeyboardLayoutId.case_insensitive_ends("-Danish"))
         {

            information("remote device changed keyboard layout to Danish");

         }
         else if (strKeyboardLayoutId.case_insensitive_ends("-ABNT2"))
         {

            information("remote device changed keyboard layout to ABNT2");

         }

      }


   }


} // namespace remoting_remoting



