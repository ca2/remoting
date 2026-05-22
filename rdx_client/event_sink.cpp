// Created by camilo on 2026-05-22 17:17 <3ThomasBorregaardSørensen!!
#include "framework.h"
#include "event_sink.h"

namespace remoting_rdx_client
{


   øBEGIN_INTERFACE_MAP(event_sink)
   øINTERFACE_MAP_ITEM(IDispatch)
   øEND_INTERFACE_MAP()

   event_sink::event_sink() {
   
   m_hwnd = nullptr;
   }

   event_sink::~event_sink() {}

   HRESULT STDMETHODCALLTYPE event_sink::Invoke(DISPID dispIdMember, REFIID, LCID, WORD, DISPPARAMS *pDispParams,
                                                VARIANT *, EXCEPINFO *, UINT *)
   {
      switch (dispIdMember)
      {
         case DISPID_CONNECTED:
            MessageBox(m_hwnd, L"Connected", L"RDP Event", MB_OK);
            break;

         case DISPID_LOGINCOMPLETE:
            MessageBox(m_hwnd, L"Login complete", L"RDP Event", MB_OK);
            break;

            // case DISPID_DISCONNECTED:
            //  MessageBox(g_hwnd, L"Disconnected", L"RDP Event", MB_OK);
            // break;

         case DISPID_FATALERROR:
            MessageBox(m_hwnd, L"Fatal error", L"RDP Event", MB_OK);
            break;

         case DISPID_DISCONNECTED:
         {
            LONG reason = 0;

            if (pDispParams && pDispParams->cArgs >= 1)
            {
               VARIANTARG &arg = pDispParams->rgvarg[0];

               if (arg.vt == VT_I4)
               {
                  reason = arg.lVal;
               }
            }

            wchar_t buf[256];

            swprintf_s(buf, L"Disconnected. Reason code = %ld", reason);

            MessageBox(m_hwnd, buf, L"RDP Disconnect", MB_OK);

            break;
         }
      }

      return S_OK;
   }


} // namespace remoting_rdx_client
 


