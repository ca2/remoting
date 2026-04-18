#pragma once


#include "aura/user/user/main_window.h"


namespace remoting_remote_connect
{


   class CLASS_DECL_REMOTING_REMOTE_CONNECT main_window :
      virtual public ::user::main_window
   {
   public:


      bool              m_dDrawControlBox;
      bool              m_dDrawOnlyMainRectangles;
      double            m_dBreathPeriod;
      class ::time      m_timeStart;
      double            m_dPhaseShift;
      int               m_iCloseButtonDraw;


      main_window();
      ~main_window() override;



      void install_message_routing(::channel * pchannel) override;


      DECLARE_MESSAGE_HANDLER(on_message_create);


      //virtual void on_create_user_interaction() override;

      virtual void _001OnDraw(::draw2d::graphics_pointer & pgraphics) override;

      virtual void _001DrawItem(::draw2d::graphics_pointer& pgraphics, ::user::item & item, const ::user::e_state & estate) override;


   };


} // namespace remoting_remote_connect



