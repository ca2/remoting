#pragma once


#include "remoting/notify_node/websockets/application.h"


namespace remoting_notify_node
{


   class CLASS_DECL_REMOTING_NOTIFY_NODE application :
      virtual public websockets::application
   {
   public:


      ::user::document *                     m_pdocMenu;
      ::user::plain_edit_impact *              m_prollfps;
      //::user::single_document_template *     m_ptemplateNotifyDrawingMain;


      application();
      ~application() override;
      
      __DECLARE_APPLICATION_RELEASE_TIME();


      virtual string preferred_experience() override;
      void init_instance() override;
      virtual void term_application() override;

      virtual void on_request(::request * prequest) override;

#ifdef _DEBUG
      virtual long long increment_reference_count() override;
      virtual long long decrement_reference_count() override;
#endif

//      virtual ::pointer<app_simple_drawing::impact> create_simple_drawing_impact(::user::impact* pimpactParent, ::user::impact_data* pimpactdata) override;


      string draw2d_get_default_implementation_name() override;



   };


} // namespace notify_drawing



