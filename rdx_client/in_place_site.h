//
// Created by camilo on 2026-05-23 10:51 <3ThomasBorregaardSørensen!! Mummi!! Bilbo!!
//
#pragma once


// ============================================================================
// IOleInPlaceSite
// ============================================================================

#include "com_base.h"


namespace windows
{


   class in_place_site :
       public IOleInPlaceSite,
       public ::windows::com_base
   {
   public:

      ::pointer < in_place_frame > m_pinplaceframe;


      øDECL_INTERFACE_MAP();

      in_place_site();
      ~in_place_site() override;

      HRESULT STDMETHODCALLTYPE GetWindow(HWND* phwnd) override;
      HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) override;
      HRESULT STDMETHODCALLTYPE CanInPlaceActivate() override;
      HRESULT STDMETHODCALLTYPE OnInPlaceActivate() override;
      HRESULT STDMETHODCALLTYPE OnUIActivate() override;
      HRESULT STDMETHODCALLTYPE GetWindowContext(
          IOleInPlaceFrame** ppFrame,
          IOleInPlaceUIWindow** ppDoc,
          LPRECT lprcPosRect,
          LPRECT lprcClipRect,
          LPOLEINPLACEFRAMEINFO lpFrameInfo);
      HRESULT STDMETHODCALLTYPE Scroll(SIZE) override;
      HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL) override;
      HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate() override;
      HRESULT STDMETHODCALLTYPE DiscardUndoState() override;
      HRESULT STDMETHODCALLTYPE DeactivateAndUndo() override;
      HRESULT STDMETHODCALLTYPE OnPosRectChange(
          LPCRECT lprcPosRect) override;
   };

} // namespace windows


