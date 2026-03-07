#include "framework.h"
#include "impact.h"
#include "application.h"
//#include "remoting/drawing/render.h"


namespace remoting_notify_node
{


   impact::impact()
   {


   }


   impact::~impact()
   {

   }


//   void impact::assert_ok() const
//   {
//
//      user::box::assert_ok();
//
//   }
//
//
//   void impact::dump(dump_context & dumpcontext) const
//   {
//
//      user::box::dump(dumpcontext);
//
//   }


   void impact::install_message_routing(::channel * psender)
   {

      ::user::impact::install_message_routing(psender);

   }


   void impact::on_layout(::draw2d::graphics_pointer & pgraphics)
   {

      auto rectangleX = this->rectangle();

      if(rectangleX.is_empty())
      {

         return;

      }

      //m_prender->m_rectangle = rectangleX * (351.0 / 1024.0);

   }


} // namespace notify_drawing



