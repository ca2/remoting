//
// Created by camilo on 2026-05-23 11:06 <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
//
#include "framework.h"
#include "com_window.h"
#include "in_place_frame.h"
#include "in_place_site.h"


namespace windows
{

   
   øBEGIN_INTERFACE_MAP(in_place_site, IOleInPlaceSite)
   øINTERFACE_MAP_ITEM(IOleInPlaceSite)
   øINTERFACE_MAP_ITEM(IOleWindow)
   øEND_INTERFACE_MAP()


   in_place_site::in_place_site()
   {

      construct_newø(m_pinplaceframe);

   }


   in_place_site::~in_place_site()
   {

   }


   HRESULT STDMETHODCALLTYPE in_place_site::GetWindow(HWND* phwnd)
   {
      auto hwnd = ::as_HWND(m_pinplaceframe->m_pcomwindow->operating_system_window());
      *phwnd = hwnd;
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::ContextSensitiveHelp(BOOL)
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::CanInPlaceActivate()
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::OnInPlaceActivate()
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::OnUIActivate()
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::GetWindowContext(
       IOleInPlaceFrame** ppFrame,
       IOleInPlaceUIWindow** ppDoc,
       LPRECT lprcPosRect,
       LPRECT lprcClipRect,
       LPOLEINPLACEFRAMEINFO lpFrameInfo)
   {

      auto hwndMain = ::as_HWND(m_pinplaceframe->m_pcomwindow->operating_system_window());
      RECT rc;
      GetClientRect(hwndMain, &rc);

      *ppFrame = m_pinplaceframe;
      (*ppFrame)->AddRef();

      *ppDoc = nullptr;

      *lprcPosRect = rc;
      *lprcClipRect = rc;

      lpFrameInfo->fMDIApp = FALSE;
      lpFrameInfo->hwndFrame = hwndMain;
      lpFrameInfo->haccel = nullptr;
      lpFrameInfo->cAccelEntries = 0;

      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::Scroll(SIZE)
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::OnUIDeactivate(BOOL)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::OnInPlaceDeactivate()
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::DiscardUndoState()
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::DeactivateAndUndo()
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_site::OnPosRectChange(
       LPCRECT lprcPosRect)
   {
      if (m_pinplaceframe->m_pinplaceobject)
      {
         m_pinplaceframe->m_pinplaceobject->SetObjectRects(
             lprcPosRect,
             lprcPosRect);
      }

      return S_OK;
   }


} // namespace windows



