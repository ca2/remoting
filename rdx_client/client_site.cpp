// Created by camilo on 2026-05-22 17:17 <3ThomasBorregaardSørensen!!
#include "framework.h"
#include "client_site.h"
#include "in_place_frame.h"
#include "in_place_site.h"
#include "com_window.h"


namespace windows
{


   client_site::client_site()
   {


   }


   client_site::~client_site()
   {


   }


   void client_site::initialize_client_site(com_window * pcomwindow)
   {

      construct_newø(m_pinplacesite);

      m_pinplacesite->m_pinplaceframe->m_pcomwindow = pcomwindow;

   }


   HRESULT STDMETHODCALLTYPE client_site::QueryInterface(REFIID riid, void** ppvObject)
   {

      if (!ppvObject)
      {

         return E_POINTER;

      }

      *ppvObject = nullptr;

      if (riid == IID_IUnknown || riid == IID_IOleClientSite)
      {

         *ppvObject = (IOleClientSite*) this;

         AddRef();

         return S_OK;

      }

      if (riid == IID_IOleInPlaceSite)
      {

         return m_pinplacesite->QueryInterface(riid, ppvObject);

      }

      return E_NOINTERFACE;

   }


   // IOleClientSite::SaveObject
   HRESULT STDMETHODCALLTYPE client_site::SaveObject() { return E_NOTIMPL; }
   // IOleClientSite::GetMoniker
   HRESULT STDMETHODCALLTYPE client_site::GetMoniker(DWORD, DWORD, IMoniker **) { return E_NOTIMPL; }
   // IOleClientSite::GetContainer
   HRESULT STDMETHODCALLTYPE client_site::GetContainer(IOleContainer **ppContainer)
   {
      *ppContainer = nullptr;
      return E_NOINTERFACE;
   }
   // IOleClientSite::ShowObject
   HRESULT STDMETHODCALLTYPE client_site::ShowObject() { return S_OK; }
   // IOleClientSite::OnShowWindow
   HRESULT STDMETHODCALLTYPE client_site::OnShowWindow(BOOL) { return S_OK; }
   // IOleClientSite::RequestNewObjectLayout
   HRESULT STDMETHODCALLTYPE client_site::RequestNewObjectLayout() { return E_NOTIMPL; }

   //
   //
   // // IOleWindow::GetWindow
   // HRESULT STDMETHODCALLTYPE client_site::GetWindow(HWND *phwnd)
   // {
   //
   //    auto hwnd = ::as_HWND(m_pcomwindow->operating_system_window());
   //    *phwnd = hwnd;
   //    return S_OK;
   // }
   //
   // // IOleWindow::ContextSensitiveHelp
   // HRESULT STDMETHODCALLTYPE client_site::ContextSensitiveHelp(BOOL) { return E_NOTIMPL; }
   //
   //
   // // IOleInPlaceSite::CanInPlaceActivate
   // HRESULT STDMETHODCALLTYPE client_site:: CanInPlaceActivate() { return S_OK; }
   // // IOleInPlaceSite::OnInPlaceActivate
   // HRESULT STDMETHODCALLTYPE client_site:: OnInPlaceActivate() { return S_OK; }
   // // IOleInPlaceSite::OnUIActivate
   // HRESULT STDMETHODCALLTYPE client_site::OnUIActivate() { return S_OK; }
   //
   //
   // // IOleInPlaceSite::GetWindowContext
   // HRESULT STDMETHODCALLTYPE client_site::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,
   //                                         LPRECT lprcPosRect, LPRECT lprcClipRect, OLEINPLACEFRAMEINFO *lpFrameInfo)
   // {
   //    *ppFrame = m_pinplacesite->m_pinplaceframe;
   //    AddRef();
   //
   //    *ppDoc = nullptr;
   //
   //    auto hwnd = ::as_HWND(m_pcomwindow->operating_system_window());
   //
   //    GetClientRect(hwnd, lprcPosRect);
   //    GetClientRect(hwnd, lprcClipRect);
   //
   //    lpFrameInfo->fMDIApp = FALSE;
   //    lpFrameInfo->hwndFrame = hwnd;
   //    lpFrameInfo->haccel = nullptr;
   //    lpFrameInfo->cAccelEntries = 0;
   //
   //    return S_OK;
   // }
   //
   // // IOleInPlaceSite::Scroll
   // HRESULT STDMETHODCALLTYPE client_site::Scroll(SIZE) { return E_NOTIMPL; }
   // // IOleInPlaceSite::OnUIDeactivate
   // HRESULT STDMETHODCALLTYPE client_site::OnUIDeactivate(BOOL) { return S_OK; }
   // // IOleInPlaceSite::OnInPlaceDeactivate
   // HRESULT STDMETHODCALLTYPE client_site::OnInPlaceDeactivate() { return S_OK; }
   // // IOleInPlaceSite::DiscardUndoState
   // HRESULT STDMETHODCALLTYPE client_site::DiscardUndoState() { return E_NOTIMPL; }
   // // IOleInPlaceSite::DeactivateAndUndo
   // HRESULT STDMETHODCALLTYPE client_site::DeactivateAndUndo() { return E_NOTIMPL; }
   //
   //
   // // IOleInPlaceSite::OnPosRectChange
   // HRESULT STDMETHODCALLTYPE client_site::OnPosRectChange(LPCRECT prcPosRect)
   // {
   //
   //    ::comptr <IOleInPlaceObject > pinplaceobject;
   //
   //    if (SUCCEEDED(m_pcomwindow->m_poleobject.as(pinplaceobject)))
   //    {
   //
   //       pinplaceobject->SetObjectRects(prcPosRect, prcPosRect);
   //
   //    }
   //
   //    return S_OK;
   //
   // }

   
   //
   // // IOleInPlaceUIWindow / Frame::GetBorder
   // HRESULT STDMETHODCALLTYPE client_site::GetBorder(LPRECT) { return E_NOTIMPL; }
   // // IOleInPlaceUIWindow / Frame::RequestBorderSpace
   // HRESULT STDMETHODCALLTYPE client_site::RequestBorderSpace(LPCBORDERWIDTHS) { return E_NOTIMPL; }
   // // IOleInPlaceUIWindow / Frame::SetBorderSpace
   // HRESULT STDMETHODCALLTYPE client_site::SetBorderSpace(LPCBORDERWIDTHS) { return S_OK; }
   // // IOleInPlaceUIWindow / Frame::SetActiveObject
   // HRESULT STDMETHODCALLTYPE client_site::SetActiveObject(IOleInPlaceActiveObject *pinplaceactiveobject, LPCOLESTR)
   // {
   //    m_pinplacesite->m_pinplaceframe->m_pinplaceactiveobject = pinplaceactiveobject;
   //    return S_OK;
   // }
   // // IOleInPlaceUIWindow / Frame::InsertMenus
   // HRESULT STDMETHODCALLTYPE client_site::InsertMenus(HMENU, LPOLEMENUGROUPWIDTHS) { return E_NOTIMPL; }
   // // IOleInPlaceUIWindow / Frame::SetMenu
   // HRESULT STDMETHODCALLTYPE client_site::SetMenu(HMENU, HOLEMENU, HWND) { return S_OK; }
   // // IOleInPlaceUIWindow / Frame::RemoveMenus
   // HRESULT STDMETHODCALLTYPE client_site::RemoveMenus(HMENU) { return S_OK; }
   // // IOleInPlaceUIWindow / Frame::SetStatusText
   // HRESULT STDMETHODCALLTYPE client_site::SetStatusText(LPCOLESTR) { return S_OK; }
   // // IOleInPlaceUIWindow / Frame::EnableModeless
   // HRESULT STDMETHODCALLTYPE client_site::EnableModeless(BOOL) { return S_OK; }
   // // IOleInPlaceUIWindow / Frame::TranslateAccelerator
   // HRESULT STDMETHODCALLTYPE client_site::TranslateAccelerator(LPMSG, WORD) { return E_NOTIMPL; }
   //
   //
   // // IOleInPlaceSiteEx::OnInPlaceActivateEx
   // HRESULT STDMETHODCALLTYPE client_site::OnInPlaceActivateEx(
   //  BOOL* pfNoRedraw,
   //  DWORD)
   // {
   //    if (pfNoRedraw)
   //       *pfNoRedraw = FALSE;
   //
   //    return S_OK;
   // }
   //
   // // IOleInPlaceSiteEx::OnInPlaceDeactivateEx
   // HRESULT STDMETHODCALLTYPE client_site::OnInPlaceDeactivateEx(BOOL)
   // {
   //    return S_OK;
   // }
   //
   // // IOleInPlaceSiteEx::RequestUIActivate
   // HRESULT STDMETHODCALLTYPE client_site::RequestUIActivate()
   // {
   //    m_pcomwindow->window_set_focus();
   //    return S_OK;
   // }
   //
   //
   // // IOleInPlaceSiteWindowless::CanWindowlessActivate
   // HRESULT STDMETHODCALLTYPE client_site::CanWindowlessActivate()
   // {
   //    return S_FALSE;
   // }
   //
   //
   // HRESULT STDMETHODCALLTYPE client_site::GetCapture() { return S_FALSE; }
   //
   // HRESULT STDMETHODCALLTYPE client_site::SetCapture(BOOL)
   // {
   //    return S_OK;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::GetFocus()
   // {
   //    return S_FALSE;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::SetFocus(BOOL)
   // {
   //    return S_OK;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::GetDC(
   //     LPCRECT,
   //     DWORD,
   //     HDC*)
   // {
   //    return E_NOTIMPL;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::ReleaseDC(HDC)
   // {
   //    return E_NOTIMPL;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::InvalidateRect(
   //     LPCRECT,
   //     BOOL)
   // {
   //    m_pcomwindow->window_invalidate_rect(nullptr, true);
   //    return S_OK;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::InvalidateRgn(
   //     HRGN,
   //     BOOL)
   // {
   //    m_pcomwindow->window_invalidate_rect(nullptr, true);
   //    return S_OK;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::ScrollRect(
   //     INT,
   //     INT,
   //     LPCRECT,
   //     LPCRECT)
   // {
   //    return E_NOTIMPL;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::AdjustRect(LPRECT)
   // {
   //    return S_OK;
   // }
   //
   // HRESULT STDMETHODCALLTYPE client_site::OnDefWindowMessage(
   //     UINT,
   //     WPARAM,
   //     LPARAM,
   //     LRESULT*)
   // {
   //    return S_FALSE;
   // }

} // namespace windows
