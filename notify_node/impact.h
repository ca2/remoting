#pragma once


#include "berg/user/user/impact.h"


namespace remoting_notify_node
{


   class CLASS_DECL_REMOTING_NOTIFY_NODE impact :
      virtual public ::user::impact
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


