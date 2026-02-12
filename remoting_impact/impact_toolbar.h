//
// Created by camilo on 2026-02-11 23:14 <3ThomasBorregaardSørensen!!
//

#pragma once

#include "gui/drawing/SolidBrush.h"
class ImpactToolbar :
virtual public ::particle
{
public:

      SolidBrush m_brushBackground;
      ::int_rectangle m_rectangle;
      bool m_bHover = false;

   ImpactToolbar();
   ~ImpactToolbar() override;

      void on_mouse(POSITION position);
      void on_draw(DeviceContext *dc);
};


