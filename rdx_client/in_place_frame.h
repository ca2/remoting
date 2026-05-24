//
// Created by camilo on 2026-05-23 10:35 <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
//
#pragma once
#include "com_base.h"


// ============================================================================
// IOleInPlaceFrame
// ============================================================================



namespace windows
{


   class in_place_frame :
      public IOleInPlaceFrame,
      virtual public ::windows::com_base
   {
   public:


      ::pointer < com_window > m_pcomwindow;
      ::comptr < IOleInPlaceActiveObject > m_pinplaceactiveobject;
      ::comptr < IOleInPlaceObject > m_pinplaceobject;


      øDECL_INTERFACE_MAP();

      HRESULT STDMETHODCALLTYPE GetWindow(HWND* phwnd) override;
      HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) override;
      HRESULT STDMETHODCALLTYPE GetBorder(LPRECT) override;
      HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS) override;
      HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS) override;
      HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject*, LPCOLESTR) override;
      HRESULT STDMETHODCALLTYPE InsertMenus(HMENU, LPOLEMENUGROUPWIDTHS) override;
      HRESULT STDMETHODCALLTYPE SetMenu(HMENU, HOLEMENU, HWND) override;
      HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU) override;
      HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR) override;
      HRESULT STDMETHODCALLTYPE EnableModeless(BOOL) override;
      HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG, WORD) override;

   };

} // namespace windows


