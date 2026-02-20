// Copyright (C) 2015 GlavSoft LLC.
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
#include "WatermarksController.h"
#include "watermark_bmp.h"
#include "remoting/remoting_common/thread/AutoLock.h"
#include "remoting/remoting_common/rfb/PixelConverter.h"

WatermarksController::WatermarksController(void)
{
}

void WatermarksController::setNewFbProperties(const ::int_rectangle &  rect, const PixelFormat & pf)
{
	setNewPixelFormat(pf);
	setNewFbSize(rect);
}

void WatermarksController::setNewFbSize(const ::int_rectangle &  rect)
{
	if (m_currentFrameBufferRect != rect || is_empty())
	{
		m_currentFrameBufferRect = rect;

		m_currentRect = frameBuffer().getDimension();
		int dx = (rect.width()/2) - (m_width/2);
		int dy = (rect.height()/2) - (m_height/2);
		m_currentRect.offset(dx, dy);
	
		m_overlay.setDimension(m_currentRect);
	}
}

void WatermarksController::setNewPixelFormat(const PixelFormat & pf)
{
	if (is_empty() || m_frameBuffer.getPixelFormat()!= pf)
	{
		FrameBuffer temp;
		FrameBuffer& fb = frameBuffer(true);

		m_overlay.setPixelFormat(pf);

		if (pf == fb.getPixelFormat())
			return;

		temp.clone(&fb);

		fb.setPixelFormat(pf);

		PixelConverter pc = PixelConverter();
		::int_rectangle rect = fb.getDimension();

		pc.setPixelFormats(fb.getPixelFormat(), temp.getPixelFormat());

		pc.convert(rect, &fb, &temp);
	}
}

void WatermarksController::showWaterMarks(FrameBuffer *frameBuffer, LocalMutex *fbLock)
{
	m_overlay.copyFrom(frameBuffer, m_currentRect.left, m_currentRect.top);

	frameBuffer->copyFrom(m_currentRect, &m_frameBuffer, 0, 0);
}

void WatermarksController::hideWatermarks(FrameBuffer *frameBuffer, LocalMutex *fbLock)
{
	frameBuffer->copyFrom(m_currentRect, &m_overlay, 0, 0);
}

const ::int_rectangle WatermarksController::CurrentRect()
{
	return m_currentRect;
}

FrameBuffer& WatermarksController::frameBuffer(bool fromFile)
{
	if (m_frameBuffer.getBuffer() == 0 || fromFile)
	{
		loadFromfile();
	}

	return m_frameBuffer;
}

void WatermarksController::loadFromfile()
{
	int pixelsOfset = *(int*)&WATERMARK_BMP_BODY[14] + 14;

	m_width = *(int*)&WATERMARK_BMP_BODY[18];
	m_height = *(int*)&WATERMARK_BMP_BODY[22];

	int imageBpp = (WATERMARK_BMP_BODY[28] + 7) / 8;

	int pad = 4 - ((m_width * 3) % 4);

	int bufferSize = m_width * m_height * 4;

	unsigned char* buffer = new unsigned char[bufferSize];


	::int_size dim(m_width, m_height);
	PixelFormat pf = StandardPixelFormatFactory::create32bppPixelFormat();
	m_frameBuffer.setPropertiesWithoutResize(dim, pf);
	m_overlay.setPropertiesWithoutResize(m_overlay.getDimension(), pf);

	for (int i = 0; i < m_height; ++i)
	{
		int vi = m_width * i * 3 + i * pad + pixelsOfset;
		int vb = m_width * (m_height - i - 1) * 4;
		for(int j = 0; j < m_width; ++j)
		{
			int hi = vi + j*3;
			int hb = vb + j*4;

			buffer[hb] = WATERMARK_BMP_BODY[hi];			//B
			buffer[hb + 1] = WATERMARK_BMP_BODY[hi + 1];	//G
			buffer[hb + 2] = WATERMARK_BMP_BODY[hi + 2];	//R
			buffer[hb + 3] = 255;							//A
		}
	}

	m_frameBuffer.setBuffer(buffer);
}

bool WatermarksController::is_empty()
{
	return m_frameBuffer.getBuffer() == 0;
}
