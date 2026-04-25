// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "DesktopServerProto.h"
#include "remoting/remoting/server_config/Configurator.h"
#include "subsystem/platform/Exception.h"

namespace remoting
{


   //DesktopServerProto::DesktopServerProto(BlockingGate *pblockinggate) : m_forwGate(pblockinggate) {}

   DesktopServerProto::DesktopServerProto()
   {


   }

   DesktopServerProto::~DesktopServerProto() {}


   void DesktopServerProto::initialize_desktop_server_proto(Configurator * pconfigurator, BlockingGate *pblockinggate)
   {

      m_pconfigurator = pconfigurator;
      m_forwGate=pblockinggate;

   }


   void DesktopServerProto::checkPixelFormat(const ::innate_subsystem::PixelFormat &pixelformat)
   {
      ::string errMess;
      if (pixelformat.bitsPerPixel != 16 && pixelformat.bitsPerPixel != 32)
      {
         errMess.formatf("Wrong value of bits per pixel ({})", (int)pixelformat.bitsPerPixel);
         throw ::subsystem::Exception(errMess);
      }
      if (pixelformat.colorDepth > pixelformat.bitsPerPixel)
      {
         errMess.formatf("Wrong value (color depth ({}) > bits per pixel ({}))", (int)pixelformat.colorDepth,
                         (int)pixelformat.bitsPerPixel);
         throw ::subsystem::Exception(errMess);
      }
   }

   void DesktopServerProto::checkRectangle(const ::int_rectangle &rect)
   {
      ::string errMess;
      if (abs(rect.left) > 32000 || abs(rect.top) > 32000 || abs(rect.right) > 32000 || abs(rect.bottom) > 32000 ||
          rect < 0)
      {
         errMess.formatf("Wrong rectangle ({}, {}, {}, {})", rect.left, rect.top, rect.right, rect.bottom);
         throw ::subsystem::Exception(errMess);
      }
   }

   void DesktopServerProto::checkDimension(const ::int_size &size)
   {
      ::string errMess;
      if (abs(size.cx) > 64000 || abs(size.cy) > 64000)
      {
         errMess.formatf("Wrong dimension (%dx{})", size.cx, size.cy);
         throw ::subsystem::Exception(errMess);
      }
   }

   void DesktopServerProto::readPixelFormat(::innate_subsystem::PixelFormat & pixelformat, BlockingGate *pblockinggate)
   {
      pixelformat.bitsPerPixel = pblockinggate->readUInt16();
      pixelformat.colorDepth = pblockinggate->readUInt16();
      pixelformat.redMax = pblockinggate->readUInt16();
      pixelformat.greenMax = pblockinggate->readUInt16();
      pixelformat.blueMax = pblockinggate->readUInt16();
      pixelformat.redShift = pblockinggate->readUInt16();
      pixelformat.greenShift = pblockinggate->readUInt16();
      pixelformat.blueShift = pblockinggate->readUInt16();
      checkPixelFormat(pixelformat);
   }

   void DesktopServerProto::sendPixelFormat(const ::innate_subsystem::PixelFormat &pixelformat, BlockingGate *pblockinggate)
   {
      pblockinggate->writeUInt16(pixelformat.bitsPerPixel);
      pblockinggate->writeUInt16(pixelformat.colorDepth);
      pblockinggate->writeUInt16(pixelformat.redMax);
      pblockinggate->writeUInt16(pixelformat.greenMax);
      pblockinggate->writeUInt16(pixelformat.blueMax);
      pblockinggate->writeUInt16(pixelformat.redShift);
      pblockinggate->writeUInt16(pixelformat.greenShift);
      pblockinggate->writeUInt16(pixelformat.blueShift);
   }

   ::int_size DesktopServerProto::readDimension(BlockingGate *pblockinggate)
   {
      ::int_size size;
      size.cx = pblockinggate->readInt32();
      size.cy = pblockinggate->readInt32();
      checkDimension(size);
      return size;
   }

   void DesktopServerProto::sendDimension(const ::int_size &size, BlockingGate *pblockinggate)
   {
      pblockinggate->writeInt32(size.cx);
      pblockinggate->writeInt32(size.cy);
   }

   ::int_point DesktopServerProto::readPoint(BlockingGate *pblockinggate)
   {
      ::int_point point;
      point.x = pblockinggate->readInt32();
      point.y = pblockinggate->readInt32();
      return point;
   }

   void DesktopServerProto::sendPoint(const ::int_point *point, BlockingGate *pblockinggate)
   {
      pblockinggate->writeInt32(point->x);
      pblockinggate->writeInt32(point->y);
   }

   ::int_rectangle DesktopServerProto::readRect(BlockingGate *pblockinggate)
   {
      ::int_rectangle rect;
      rect.left = pblockinggate->readInt32();
      rect.top = pblockinggate->readInt32();
      rect.set_width(pblockinggate->readInt32());
      rect.set_height(pblockinggate->readInt32());

      checkRectangle(rect);
      return rect;
   }

   void DesktopServerProto::sendRect(const ::int_rectangle &rect, BlockingGate *pblockinggate)
   {
      pblockinggate->writeInt32(rect.left);
      pblockinggate->writeInt32(rect.top);
      pblockinggate->writeInt32(rect.width());
      pblockinggate->writeInt32(rect.height());
   }

   void DesktopServerProto::sendRegion(const Region *region, BlockingGate *pblockinggate)
   {
      ::int_rectangle_array_base rects;
      ::int_rectangle_array_base::iterator iRect;
      region->getRectVector(&rects);

      unsigned int numRects = (unsigned int)rects.size();
      _ASSERT(numRects == rects.size());
      pblockinggate->writeUInt32(numRects);

      for (iRect = rects.begin(); iRect < rects.end(); iRect++)
      {
         auto & rect = (*iRect);
         sendRect(rect, pblockinggate);
      }
   }

   void DesktopServerProto::readRegion(Region *region, BlockingGate *pblockinggate)
   {
      region->clear();
      unsigned int rectCount = pblockinggate->readUInt32();
      for (unsigned int i = 0; i < rectCount; i++)
      {
         ::int_rectangle r = readRect(pblockinggate);
         if (r.is_ok())
         {
            region->addRect(r);
         }
      }
   }

   void DesktopServerProto::sendFrameBuffer(const ::innate_subsystem::FrameBuffer *pframebufferSource,
                                            const ::int_rectangle &srcRect, BlockingGate *pblockinggate)
   {
      // FIXME: Additional ::innate_subsystem::FrameBuffer will be used temporarily.
      // This is easy way to send all pixels.
      ::innate_subsystem::PixelFormat pixelformat = pframebufferSource->getPixelFormat();
      ::innate_subsystem::FrameBuffer pframebuffer;

      pframebuffer.setProperties(srcRect, pixelformat);
      pframebuffer.copyFrom(pframebufferSource, srcRect.left, srcRect.top);

      pblockinggate->write(pframebuffer.getBuffer(), pframebuffer.getBufferSize());
   }

   void DesktopServerProto::readFrameBuffer(::innate_subsystem::FrameBuffer *pframebufferTarget, const ::int_rectangle &dstRect,
                                            BlockingGate *pblockinggate)
   {
      // FIXME: ::innate_subsystem::FrameBuffer will be used temporarily.
      // This is easy way to get all pixels.
      ::innate_subsystem::PixelFormat pixelformat = pframebufferTarget->getPixelFormat();
      ::innate_subsystem::FrameBuffer pframebuffer;
      pframebuffer.setProperties(dstRect, pixelformat);

      pblockinggate->readFully(pframebuffer.getBuffer(), pframebuffer.getBufferSize());
      pframebufferTarget->copyFrom(dstRect, &pframebuffer, 0, 0);
   }

   void DesktopServerProto::sendNewClipboard(const ::scoped_string &newClipboard, BlockingGate *pblockinggate)
   {
      pblockinggate->writeUTF8(newClipboard);
   }

   void DesktopServerProto::readNewClipboard(::string &newClipboard, BlockingGate *pblockinggate)
   {
      newClipboard = pblockinggate->readUtf8();
   }

   void DesktopServerProto::sendNewPointerPos(const ::int_point newPos, unsigned char keyFlag, BlockingGate *pblockinggate)
   {
      // Send pointer position
      pblockinggate->writeUInt16(newPos.x);
      pblockinggate->writeUInt16(newPos.y);
      // Send key flags
      pblockinggate->writeUInt8(keyFlag);
   }

   void DesktopServerProto::readNewPointerPos(::int_point *newPos, unsigned char *keyFlag, BlockingGate *pblockinggate)
   {
      // Read pointer position
      newPos->x = pblockinggate->readUInt16();
      newPos->y = pblockinggate->readUInt16();
      // Read key flags
      *keyFlag = pblockinggate->readUInt8();
   }

   void DesktopServerProto::sendKeyEvent(unsigned int keySym, bool down, BlockingGate *pblockinggate)
   {
      pblockinggate->writeUInt32(keySym);
      pblockinggate->writeUInt8((unsigned char)down);
   }

   void DesktopServerProto::readKeyEvent(unsigned int *keySym, bool *down, BlockingGate *pblockinggate)
   {
      *keySym = pblockinggate->readUInt32();
      *down = pblockinggate->readUInt8() != 0;
   }

   void DesktopServerProto::sendUserInfo(const ::scoped_string &desktopName, const ::scoped_string &userName,
                                         BlockingGate *pblockinggate)
   {
      pblockinggate->writeUTF8(desktopName);
      pblockinggate->writeUTF8(userName);
   }

   void DesktopServerProto::readUserInfo(::string &desktopName, ::string &userName, BlockingGate *pblockinggate)
   {
      desktopName = pblockinggate->readUtf8();
      userName = pblockinggate->readUtf8();
   }

   void DesktopServerProto::sendConfigSettings(BlockingGate *pblockinggate)
   {
      auto srvConf = m_pconfigurator->getServerConfig();

      // Log
      pblockinggate->writeUInt32(srvConf->getLogLevel());

      // Polling
      pblockinggate->writeUInt32(srvConf->getPollingInterval());
      pblockinggate->writeUInt8(srvConf->getGrabTransparentWindowsFlag());

      //
      pblockinggate->writeUInt8(srvConf->isBlockingLocalInput());
      pblockinggate->writeUInt8(srvConf->isLocalInputPriorityEnabled());
      pblockinggate->writeUInt32(srvConf->getLocalInputPriorityTimeout());

      pblockinggate->writeUInt8(srvConf->isRemovingDesktopWallpaperEnabled());

      // Send video class names
      AutoLock al(srvConf);
      ::string_array *wndClassNames = srvConf->getVideoClassNames();
      ::string_array::iterator iter = wndClassNames->begin();
      size_t stringCount = wndClassNames->size();
      pblockinggate->writeUInt32((unsigned int)stringCount);
      for (; iter < wndClassNames->end(); iter++)
      {
         pblockinggate->writeUTF8((*iter));
      }
      // Send video rects
      ::int_rectangle_array_base *Rects = srvConf->getVideoRects();
      size_t size = Rects->size();
      pblockinggate->writeUInt32((unsigned int)size);
      for (size_t i = 0; i < size; i++)
      {
         ::string s;
         RectSerializer::toString((Rects->at(i)), s);
         pblockinggate->writeUTF8(s);
      }
      // Send video recognition interval
      pblockinggate->writeUInt32(srvConf->getVideoRecognitionInterval());
      // Send socket timeout
      pblockinggate->writeUInt32(srvConf->getIdleTimeout());
   }

   void DesktopServerProto::readConfigSettings(BlockingGate *pblockinggate)
   {
      ServerConfig *srvConf = m_pconfigurator->getServerConfig();

      // Log
      srvConf->setLogLevel(pblockinggate->readUInt32());

      // Polling
      srvConf->setPollingInterval(pblockinggate->readUInt32());
      srvConf->setGrabTransparentWindowsFlag(pblockinggate->readUInt8() != 0);

      srvConf->blockLocalInput(pblockinggate->readUInt8() != 0);
      srvConf->setLocalInputPriority(pblockinggate->readUInt8() != 0);
      srvConf->setLocalInputPriorityTimeout(pblockinggate->readUInt32());

      srvConf->enableRemovingDesktopWallpaper(pblockinggate->readUInt8() != 0);

      // Receive video class names
      AutoLock al(srvConf);
      size_t stringCount = pblockinggate->readUInt32();

      ::string tmpString;
      for (size_t i = 0; i < stringCount; i++)
      {
         tmpString= pblockinggate->readUtf8();
         srvConf->getVideoClassNames()->add(tmpString);
      }
      // Receive video rects
      stringCount = pblockinggate->readUInt32();

      tmpString = "";
      for (size_t i = 0; i < stringCount; i++)
      {
         tmpString = pblockinggate->readUtf8();
         srvConf->getVideoRects()->add(RectSerializer::toRect(tmpString));
      }

      // Receive video recognition interval
      srvConf->setVideoRecognitionInterval(pblockinggate->readUInt32());
      // Receive socket timeout
      srvConf->setIdleTimeout(pblockinggate->readUInt32());
   }


} // namespace remoting
