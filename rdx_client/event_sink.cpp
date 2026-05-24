// Created by camilo on 2026-05-22 17:17 <3ThomasBorregaardSørensen!!
#include "framework.h"
#include "event_sink.h"

#include "main_window.h"

void DumpWindowHierarchy(HWND hwnd);

namespace remoting_rdx_client
{
   //
   //
   // øBEGIN_INTERFACE_MAP(event_sink, RDP::IMsTscAxEvents)
   // øINTERFACE_MAP_ITEM(RDP::IMsTscAxEvents)
   // øINTERFACE_MAP_ITEM(IDispatch)
   // øEND_INTERFACE_MAP()
   //
   // event_sink::event_sink()
   // {
   //
   //
   // }
   //
   //
   // event_sink::~event_sink()
   // {
   //
   //
   // }
   //
   //
   //
   // HRESULT STDMETHODCALLTYPE event_sink::QueryInterface(
   //  REFIID riid,
   //  void** ppvObject)
   // {
   //    if (!ppvObject)
   //       return E_POINTER;
   //
   //    *ppvObject = nullptr;
   //
   //    if (riid == IID_IUnknown ||
   //        riid == IID_IDispatch ||
   //        riid == m_refiid)
   //    {
   //       *ppvObject =
   //           static_cast<IDispatch*>(this);
   //
   //       AddRef();
   //       return S_OK;
   //    }
   //
   //    return E_NOINTERFACE;
   // }
   //
   //
   // // void event_sink::initialize_event_sink(main_window * pmainwindow)
   // // {
   // //
   // //    m_pmainwindow = pmainwindow;
   // //
   // // }
   //
   //
   // HRESULT STDMETHODCALLTYPE event_sink::GetTypeInfoCount(UINT * pctinfo)
   // {
   //
   //    if (pctinfo)
   //    {
   //
   //       *pctinfo = 0;
   //
   //    }
   //
   //    return S_OK;
   //
   // }
   //
   //
   // HRESULT STDMETHODCALLTYPE event_sink::GetTypeInfo(UINT, LCID, ITypeInfo **)
   // {
   //
   //    return E_NOTIMPL;
   //
   // }
   //
   //
   // HRESULT STDMETHODCALLTYPE event_sink::GetIDsOfNames(
   //  REFIID,
   //  LPOLESTR*,
   //  UINT,
   //  LCID,
   //  DISPID*)
   // {
   //
   //    return DISP_E_UNKNOWNNAME;
   //
   // }
   //
   //
   // HRESULT STDMETHODCALLTYPE event_sink::Invoke(DISPID dispIdMember, REFIID, LCID, WORD, DISPPARAMS *pDispParams,
   //                                              VARIANT *, EXCEPINFO *, UINT *)
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
   //

} // namespace remoting_rdx_client
 


