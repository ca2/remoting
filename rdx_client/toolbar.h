//
// Created by camilo on 2026-05-24 08:50 <ThomasBorregaardSørensen!! Mummi!! Bilbo!!
// //
#pragma once


#include <Gdiplus.h>

using namespace Gdiplus;

#include "acme/user/user/interaction.h"


#define WM_TOOLBAR_CMD (WM_APP + 100)


namespace remoting_rdx_client
{


   class CLASS_DECL_REMOTING_RDX_CLIENT toolbar :
   virtual public ::acme::user::interaction
   {
   public:

      ::pointer < ::acme::user::interaction > m_pacmeuserinteractionMain;

      //HWND m_hwnd{};

      static constexpr wchar_t CLASS_NAME[] =
          L"RdpOverlayToolbarGdiPlusClass";

      ::string m_strTitle;

      //int m_x{};
      //int m_y{};
      //int m_width{};
      //int m_height{};

      ::i32_rectangle m_rectangle;
      const float m_btnSize = 18.f;
      const float m_btnSpacing = 2.f;

      bool m_bMouseEnable = true;
      RectF m_titleRect;
      RectF m_rcMin;
      RectF m_rcRestore;
      RectF m_rcClose;

      static HHOOK s_mouseHook;
      static toolbar* s_instance;




      bool m_hoverMin = false;
      bool m_hoverRestore = false;
      bool m_hoverClose = false;

      bool m_mouseDown = false;

      ULONG_PTR m_gdiplusToken{};


      toolbar();
      ~toolbar() override;


      static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

      ::windows::window_class _get_window_class() override;

      ::i64 get_style_for_creating_window() override;
      ::i64 get_ex_style_for_creating_window() override;

      ::i32_rectangle get_rectangle() override;

      void on_global_mouse(const ::i32_point & point, ::wparam wparam);

      void InstallMouseHook();
      void UninstallMouseHook();

   //   virtual float get_window_scale();

      //bool Create(::windows::com_window * pcomwindowParent, int x, int y, int width, int height);
      void Destroy();

      //void Show(bool show);
      void Move(int x, int y, int width, int height);

      void Redraw();

      //HWND GetHwnd() const { return m_hwnd; }
      virtual void PostToHost(int cmd);


      bool InitGdiplus();
      void ShutdownGdiplus();

      bool RegisterClass();


      bool _on_window_procedure(lresult &lresult, u32 message, wparam wparam, lparam lparam) override;

      // static LRESULT CALLBACK WndProc(
      //     HWND hwnd,
      //     UINT msg,
      //     WPARAM wParam,
      //     LPARAM lParam);
      //
      // LRESULT HandleMessage(
      //     UINT msg,
      //     WPARAM wParam,
      //     LPARAM lParam);
      //
      virtual void RenderLayered();

      virtual void _on_draw(::Gdiplus::Graphics * pgraphics);

   };


} // namespace remoting_rdx_client


