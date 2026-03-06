#include "framework.h"
#include "application.h"
#include "main_frame.h"
#include "impact.h"
#include "acme/handler/request.h"
#include "acme/platform/system.h"
#include "app-simple/drawing/document.h"
#include "app-simple/drawing/tab_impact.h"
#include "berg/user/user/single_document_template.h"


__IMPLEMENT_APPLICATION_RELEASE_TIME(app_simple_notify_drawing);
IMPLEMENT_APPLICATION_FACTORY(app_simple_notify_drawing);


namespace app_simple_notify_drawing
{


   // __IMPLEMENT_APPLICATION_RELEASE_TIME1(app_simple_notify_drawing));


   application::application()
   {

      //m_ptemplateNotifyDrawingMain = NULL;

      m_strAppId = "app-simple/notify_drawing";
      m_strBaseSupportId = "ca2_flag";
      m_strAppName = "Notify Drawing";
      m_bLicense = false;

   }


   application::~application()
   {
   }


   void application::init_instance()
   {

      //set_local_data();

      factory()->add_factory_item <::app_simple_notify_drawing::main_frame >();
      factory()->add_factory_item <::app_simple_notify_drawing::impact >();

      ::app_simple_drawing::application::init_instance();

      //if (!::app_simple_drawing::application::init_instance())
      //{

      //   return false;

      //}

      add_matter_locator("app-simple/drawing");

      //auto pdoctemplate =
      add_impact_system(
         "main", __initialize_new ::user::single_document_template(
                               "main",
                               typeid(app_simple_drawing::document ),
                               typeid(main_frame ),
                               typeid(app_simple_drawing::tab_impact )));
      //m_ptemplateNotifyDrawingMain = pdoctemplate;
      //add_document_template(pdoctemplate);


//      pdoctemplate = __initialize_new ::user::single_document_template(
//                          this,
//                          "main",
//                          typeid(simple_drawing::document ),
//                          typeid(simple_drawing::frame ),
//                          typeid(simple_drawing::main_impact )));
//
//      m_ptemplateNotifyDrawingImpact = pdoctemplate;
//
//      add_document_template(pdoctemplate);

      //return true;

   }


   void application::term_application()
   {

      ::aura::application::term_application();

   }


   void application::on_request(::request * prequest)
   {

#if 0
      {

         auto pthread = fork([this]()
            {

               informationf("test");

            });

      }
#endif

#if 0

#ifdef _DEBUG

      informationf("_DEBUG build? (basis)");

      ASSERT(false);

#else

      informationf("RELEASE build? (stage)");

      ASSERT(false);

#endif

#endif

      m_bMultiverseChat = !is_true("no_hello_edit");

      if (impact_system("main")->get_document_count() == 0)
      {

         impact_system("main")->request(prequest);

      }

      if (is_true("wfi_maximize"))
      {

         prequest->payload("document").cast < app_simple_drawing::document >()->get_typed_impact < ::user::tab_impact >()->top_level_frame()->design_window_maximize();

      }

      informationf("----> finished notify_drawing::on_request");

   }


   string application::preferred_experience()
   {

      //return "aura";

      return ::aura::application::preferred_experience();

   }


#ifdef _DEBUG


   long long application::increment_reference_count()
   {

      return ::object::increment_reference_count();

   }


   long long application::decrement_reference_count()
   {

      return ::object::decrement_reference_count();

   }


#endif


   ::pointer<app_simple_drawing::impact> application::create_simple_drawing_impact(::user::impact* pimpactParent, ::user::impact_data* pimpactdata)
   {

      return pimpactParent->create_impact < impact >(pimpactdata);

   }


   string application::draw2d_get_default_implementation_name()
   {

      //return system()->implementation_name("draw2d", "opengl");

      //return system()->implementation_name("draw2d", "gdiplus");

      return ::app_simple_drawing::application::draw2d_get_default_implementation_name();

   }

   
} // namespace notify_drawing



