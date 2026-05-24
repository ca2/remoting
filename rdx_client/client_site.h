// Created by camilo on 2026-05-22 18:07 <3ThomasBorregaardSørensen!!
#pragma once


#include "com_base.h"


namespace windows
{


   class CLASS_DECL_REMOTING_RDX_CLIENT client_site :
      public IOleClientSite,
      virtual public ::windows::com_base
   {
   public:


      ::pointer < in_place_site > m_pinplacesite;


      client_site();
      ~client_site() override;


      virtual void  initialize_client_site(com_window * pcomwindow);



      HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
      ULONG STDMETHODCALLTYPE AddRef() { return InternalAddRef(); }
      ULONG STDMETHODCALLTYPE Release() { return InternalRelease(); }


      // IOleClientSite
      HRESULT STDMETHODCALLTYPE SaveObject();
      HRESULT STDMETHODCALLTYPE GetMoniker(DWORD, DWORD, IMoniker **);
      HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer **ppContainer);
      HRESULT STDMETHODCALLTYPE ShowObject();
      HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL);
      HRESULT STDMETHODCALLTYPE RequestNewObjectLayout();


   };


} // namespace remoting_rdx_client