// Created by camilo on 2026-05-22 17:14 <3ThomasBorregaardSørensen!!
#pragma once


namespace remoting_rdx_client
{

   class event_sink : public IDispatch,
      virtual public ::particle
   {

   public:


      HWND m_hwnd;

      event_sink();
      ~event_sink();


      void _set_hwnd(HWND hwnd);


      øDECL_INTERFACE_MAP();
      //HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv)
      //{
      //   if (!ppv)
      //      return E_POINTER;

      //   *ppv = nullptr;


      //   return E_NOINTERFACE;
      //}


      HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *) { return E_NOTIMPL; }
      HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo **) { return E_NOTIMPL; }
      HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR *, UINT, LCID, DISPID *) { return E_NOTIMPL; }

      HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID, LCID, WORD, DISPPARAMS *pDispParams, VARIANT *,
                                       EXCEPINFO *, UINT *) override;

   };


} // namespace remoting rdx_client
 


