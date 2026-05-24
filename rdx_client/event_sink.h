// Created by camilo on 2026-05-22 17:14 <3ThomasBorregaardSørensen!!
#pragma once

#include "com_base.h"

#include <comdef.h>
#include <ocidl.h>
#include <oleidl.h>


namespace windows
{

   template < typename EVENTS >
   class event_sink :
      public IDispatch,
      virtual public ::windows::com_base
   {

   public:


      ::comptr < IConnectionPoint  > m_pconnectionpoint;

DWORD m_dwCookie;

      event_sink()
      {


      }
      ~event_sink()
      {


      }


      void initialize_event_sink(IConnectionPointContainer * pconnectionpointcontainer)
      {

         //m_prdxclient->m_prdpclient.as(m_pinplaceactiveobject);

         auto hr = pconnectionpointcontainer->FindConnectionPoint(__uuidof(EVENTS), &m_pconnectionpoint);

         if (FAILED(hr))
         {

            MessageBox(nullptr, L"FindConnectionPoint failed", L"Error", MB_ICONERROR);


            return;
         }

         //construct_newø(m_peventsink);

         HRESULT hrAdvise = m_pconnectionpoint->Advise((IUnknown *)(::windows::event_sink<EVENTS>*)this, &m_dwCookie);

         if (FAILED(hrAdvise))
         {

            ::string strMessage = hresult_to_string(hrAdvise);

            auto pszMessage = strMessage.c_str();

            error(pszMessage);

         }



      }


   HRESULT STDMETHODCALLTYPE QueryInterface(
    REFIID riid,
    void** ppvObject)
   {
      if (!ppvObject)
         return E_POINTER;

      *ppvObject = nullptr;

      if (riid == IID_IUnknown ||
          riid == IID_IDispatch ||
          riid == __uuidof(EVENTS))
      {
         *ppvObject =
             static_cast<IDispatch*>(this);

         AddRef();
         return S_OK;
      }

      return E_NOINTERFACE;
   }


         ULONG STDMETHODCALLTYPE AddRef() { return InternalAddRef(); }
   ULONG STDMETHODCALLTYPE Release() { return InternalRelease(); }



   // void event_sink::initialize_event_sink(main_window * pmainwindow)
   // {
   //
   //    m_pmainwindow = pmainwindow;
   //
   // }


   HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT * pctinfo)
   {

      if (pctinfo)
      {

         *pctinfo = 0;

      }

      return S_OK;

   }


   HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo **)
   {

      return E_NOTIMPL;

   }


   HRESULT STDMETHODCALLTYPE GetIDsOfNames(
    REFIID,
    LPOLESTR*,
    UINT,
    LCID,
    DISPID*)
   {

      return DISP_E_UNKNOWNNAME;

   }


   HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID, LCID, WORD, DISPPARAMS *pDispParams,
                                                VARIANT *, EXCEPINFO *, UINT *)
   {
      return S_OK;
   }
   // {
   //    switch (dispIdMember)
   //    {
   //       case DISPID_CONNECTED:
   //          //MessageBox(m_hwnd, L"Connected", L"RDP Event", MB_OK);
   //          information("RDP Event :: Connected");
   //          m_pmainwindow->on_rdp_connected();
   //          break;
   //
   //       case DISPID_LOGINCOMPLETE:
   //          //information("RDP Event :: Connected");MessageBox(m_hwnd, L"Login complete", L"RDP Event", MB_OK);
   //
   //          information("RDP Event :: Login Complete");
   //          m_pmainwindow->on_rdp_login_complete();
   //          break;
   //          // case DISPID_DISCONNECTED:
   //          //  MessageBox(g_hwnd, L"Disconnected", L"RDP Event", MB_OK);
   //          // break;
   //
   //       case DISPID_FATALERROR:
   //       {
   //
   //          auto hwnd = ::as_HWND(m_pmainwindow->operating_system_window());
   //
   //          MessageBox(hwnd, L"Fatal error", L"RDP Event", MB_OK);
   //
   //       }
   //          break;
   //
   //       case DISPID_DISCONNECTED:
   //       {
   //
   //          LONG reason = 0;
   //
   //          if (pDispParams && pDispParams->cArgs >= 1)
   //          {
   //
   //             VARIANTARG & arg = pDispParams->rgvarg[0];
   //
   //             if (arg.vt == VT_I4)
   //             {
   //
   //                reason = arg.lVal;
   //
   //             }
   //
   //          }
   //
   //          auto hwnd = ::as_HWND(m_pmainwindow->operating_system_window());
   //
   //          wchar_t buf[256];
   //
   //          swprintf_s(buf, L"Disconnected. Reason code = %ld", reason);
   //
   //          MessageBox(hwnd, buf, L"RDP Disconnect", MB_OK);
   //
   //       }
   //       break;
   //    }
   //
   //    return S_OK;
   // }


      void close_event_sink()
      {

         if (m_pconnectionpoint && m_dwCookie != 0)
         {
            m_pconnectionpoint->Unadvise(m_dwCookie);
            m_dwCookie = 0;
         }

      }

   };


} // namespace windows
 


