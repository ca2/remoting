#pragma once


#include "app-simple/drawing/impact.h"


namespace app_simple_notify_drawing
{


   class CLASS_DECL_APP_SIMPLE_NOTIFY_DRAWING impact :
      virtual public ::app_simple_drawing::impact
   {
   public:


      impact();
      ~impact() override;

//      // void assert_ok() const override;
//      // void dump(dump_context & dumpcontext) const override;

      void install_message_routing(::channel * psender) override;

      void on_layout(::draw2d::graphics_pointer & pgraphics) override;

   };


} // namespace notify_drawing


