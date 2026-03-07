#pragma once


#include "berg/user/user/tab_impact.h"
#include "aura/user/user/form_callback.h"
#include "apex/platform/app_consumer.h"


namespace remoting_notify_node
{


   class CLASS_DECL_REMOTING_NOTIFY_NODE tab_impact :
      virtual public app_interaction < application, ::user::tab_impact >,
      virtual public ::user::form_callback
   {
   public:


      ::user::split_impact *                            m_ptopviewLast;
      ::pointer<impact>       m_pimpactDrawing;
      string                                          m_strTopicTitle;


      ::pointer<::user::document>               m_pdocMenu;


      tab_impact();
      ~tab_impact() override;


      void on_create_impact(::user::impact_data * pimpactdata) override;

      void install_message_routing(::channel * pchannel) override;

      // void assert_ok() const override;

      // void dump(dump_context & dumpcontext) const override;


      DECLARE_MESSAGE_HANDLER(on_message_create);


      void on_change_cur_sel() override;

      void handle(::topic * ptopic, ::handler_context * phandlercontext) override;


   };


} // namespace remoting_notify_node




