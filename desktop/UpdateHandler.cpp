// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "UpdateHandler.h"

UpdateHandler::UpdateHandler()
{
}

UpdateHandler::~UpdateHandler(void)
{
}

void UpdateHandler::initFrameBuffer(const FrameBuffer *newFb)
{
  AutoLock al(&m_fbLocMut);
  m_backupFrameBuffer.clone(newFb);
}

bool UpdateHandler::updateExternalFrameBuffer(FrameBuffer *fb, const Region *region,
                                              const ::int_rectangle &  viewPort)
{
  AutoLock al(&m_fbLocMut);
  return updateExternalFrameBuffer(fb, &m_backupFrameBuffer, region, viewPort);
}

bool UpdateHandler::updateExternalFrameBuffer(FrameBuffer *dstFb, FrameBuffer *srcFb,
                                              const Region *region,
                                              const ::int_rectangle &  viewPort)
{
  PixelFormat dstPf = dstFb->getPixelFormat();
  PixelFormat srcPf = srcFb->getPixelFormat();
  ::int_size dstFbDim = dstFb->getDimension();
  ::int_rectangle srcFbRect = srcFb->getDimension();
  ::int_rectangle resultViewPort = srcFbRect.intersection(viewPort);

  if (!dstPf.isEqualTo(&srcPf) || !dstFbDim.isEqualTo(&::int_size(&resultViewPort)) ||
      !resultViewPort.isEqualTo(viewPort)) {
    dstFb->setProperties(&resultViewPort, &srcPf);
    return false;
  }

  ::array_base<::int_rectangle> rects;
  ::array_base<::int_rectangle>::iterator iRect;
  region->getRectVector(&rects);

  for (iRect = rects.begin(); iRect < rects.end(); iRect++) {
    ::int_rectangle *rect = &(*iRect);
    dstFb->copyFrom(rect, srcFb,
                           rect.left + viewPort.left,
                           rect.top + viewPort.top);
  }
  return true;
}
