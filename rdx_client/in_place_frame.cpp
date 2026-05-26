//
// Created by camilo on 2026-05-23 10:38 <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
//
#include "framework.h"
#include "com_window.h"
#include "in_place_frame.h"


namespace windows
{

   øBEGIN_INTERFACE_MAP(in_place_frame, IOleInPlaceFrame)
   øINTERFACE_MAP_ITEM(IOleInPlaceFrame)
   øINTERFACE_MAP_ITEM(IOleInPlaceUIWindow)
   øINTERFACE_MAP_ITEM(IOleWindow)
   øEND_INTERFACE_MAP()


      ::function<bool(MSG *)> in_place_frame::get_translator_handler()
   {

      if (!m_translatorhandler)
      {

         m_translatorhandler = [this](MSG *pmsg) -> bool
         {
            auto hr = m_pinplaceactiveobject->TranslateAccelerator(pmsg);

            auto handled = (hr == S_OK);

            return handled;
         };


      }

      return m_translatorhandler;

   }


   HRESULT STDMETHODCALLTYPE in_place_frame::GetWindow(HWND* phwnd)
   {
      
      auto hwnd = ::as_HWND(m_pcomwindow->operating_system_window());
      *phwnd = hwnd;
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::ContextSensitiveHelp(BOOL)
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::GetBorder(LPRECT)
   {
      return INPLACE_E_NOTOOLSPACE;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::RequestBorderSpace(LPCBORDERWIDTHS)
   {
      return INPLACE_E_NOTOOLSPACE;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::SetBorderSpace(LPCBORDERWIDTHS)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::SetActiveObject(
       IOleInPlaceActiveObject*pinplaceactiveobject,
       LPCOLESTR)
   {
      m_pinplaceactiveobject = pinplaceactiveobject;
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::InsertMenus(
       HMENU,
       LPOLEMENUGROUPWIDTHS)
   {
      return E_NOTIMPL;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::SetMenu(
       HMENU,
       HOLEMENU,
       HWND)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::RemoveMenus(HMENU)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::SetStatusText(LPCOLESTR)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::EnableModeless(BOOL)
   {
      return S_OK;
   }

   HRESULT STDMETHODCALLTYPE in_place_frame::TranslateAccelerator(
       LPMSG,
       WORD)
   {
      return E_NOTIMPL;
   }
} // namespace windows