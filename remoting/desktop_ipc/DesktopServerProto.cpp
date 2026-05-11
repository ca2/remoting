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
#include "remoting/remoting/node_config/Configurator.h"
#include "subsystem/platform/Exception.h"

namespace remoting
{


   //DesktopServerProto::DesktopServerProto(BlockingGate *pblockinggate) : m_pcontrolgate(pcontrolgate) {}

   DesktopServerProto::DesktopServerProto()
   {


   }

   DesktopServerProto::~DesktopServerProto() {}


   void DesktopServerProto::initialize_desktop_server_proto(::remoting_node::Configurator * pconfigurator, BlockingGate *pblockinggate)
   {

      m_pconfigurator = pconfigurator;
      m_pcontrolgate=pblockinggate;

   }


   void DesktopServerProto::checkPixelFormat(const ::innate_subsystem::PixelFormat &pixelformat)
   {
      ::string errMess;
      if (pixelformat.bitsPerPixel != 16 && pixelformat.bitsPerPixel != 32)
      {
         errMess.format("Wrong value of bits per pixel ({})", (int)pixelformat.bitsPerPixel);
         throw ::subsystem::Exception(errMess);
      }
      if (pixelformat.colorDepth > pixelformat.bitsPerPixel)
      {
         errMess.format("Wrong value (color depth ({}) > bits per pixel ({}))", (int)pixelformat.colorDepth,
                         (int)pixelformat.bitsPerPixel);
         throw ::subsystem::Exception(errMess);
      }
   }

   void DesktopServerProto::checkRectangle(const ::int_rectangle & rectangle)
   {
      ::string errMess;
      if (abs(rectangle.left) > 32000 || abs(rectangle.top) > 32000 || abs(rectangle.right) > 32000 || abs(rectangle.bottom) > 32000 ||
          rectangle < 0)
      {
         errMess.format("Wrong rectangle ({}, {}, {}, {})", rectangle.left, rectangle.top, rectangle.right, rectangle.bottom);
         throw ::subsystem::Exception(errMess);
      }
   }

   void DesktopServerProto::checkDimension(const ::int_size &size)
   {
      ::string errMess;
      if (abs(size.cx) > 64000 || abs(size.cy) > 64000)
      {
         errMess.format("Wrong dimension ({}x{})", size.cx, size.cy);
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

   void DesktopServerProto::sendPoint(const ::int_point &point, BlockingGate *pblockinggate)
   {
      pblockinggate->writeInt32(point.x);
      pblockinggate->writeInt32(point.y);
   }

   ::int_rectangle DesktopServerProto::readRect(BlockingGate *pblockinggate)
   {
      ::int_rectangle rectangle;
      rectangle.left = pblockinggate->readInt32();
      rectangle.top = pblockinggate->readInt32();
      rectangle.set_width(pblockinggate->readInt32());
      rectangle.set_height(pblockinggate->readInt32());

      checkRectangle(rectangle);
      return rectangle;
   }

   void DesktopServerProto::sendRect(const ::int_rectangle & rectangle, BlockingGate *pblockinggate)
   {
      pblockinggate->writeInt32(rectangle.left);
      pblockinggate->writeInt32(rectangle.top);
      pblockinggate->writeInt32(rectangle.width());
      pblockinggate->writeInt32(rectangle.height());
   }

   void DesktopServerProto::sendRegion(const Region & region, BlockingGate *pblockinggate)
   {
      //::int_rectangle_array_base rectanglea;
      ::int_rectangle_array_base::iterator iRect;
      auto rectanglea = region.getRects();

      unsigned int numRects = (unsigned int)rectanglea.size();
      ASSERT(numRects == rectanglea.size());
      pblockinggate->writeUInt32(numRects);

      for (iRect = rectanglea.begin(); iRect < rectanglea.end(); iRect++)
      {
         auto & rectangle = (*iRect);
         sendRect(rectangle, pblockinggate);
      }
   }

   void DesktopServerProto::readRegion(Region & region, BlockingGate *pblockinggate)
   {
      region.clear();
      unsigned int rectCount = pblockinggate->readUInt32();
      for (unsigned int i = 0; i < rectCount; i++)
      {
         ::int_rectangle r = readRect(pblockinggate);
         if (r.is_ok())
         {
            region.addRect(r);
         }
      }
   }

   void DesktopServerProto::sendFramebuffer(const ::innate_subsystem::Framebuffer *pframebufferSource,
                                            const ::int_rectangle &rectangleSource, BlockingGate *pblockinggate)
   {
      // FIXME: Additional ::innate_subsystem::Framebuffer will be used temporarily.
      // This is easy way to send all pixels.
      ::innate_subsystem::PixelFormat pixelformat = pframebufferSource->getPixelFormat();
      auto pframebuffer = createø<::innate_subsystem::Framebuffer>();

      pframebuffer->setProperties(rectangleSource, pixelformat);
      pframebuffer->copyFrom(pframebufferSource, rectangleSource.left, rectangleSource.top);

      pblockinggate->write(pframebuffer->getBuffer(), pframebuffer->getBufferSize());
   }

   void DesktopServerProto::readFramebuffer(::innate_subsystem::Framebuffer *pframebufferTarget, const ::int_rectangle &rectangleTarget,
                                            BlockingGate *pblockinggate)
   {
      // FIXME: ::innate_subsystem::Framebuffer will be used temporarily.
      // This is easy way to get all pixels.
      ::innate_subsystem::PixelFormat pixelformat = pframebufferTarget->getPixelFormat();
      auto pframebuffer = createø<::innate_subsystem::Framebuffer>();
      pframebuffer->setProperties(rectangleTarget, pixelformat);

      pblockinggate->readFully(pframebuffer->getBuffer(), pframebuffer->getBufferSize());
      pframebufferTarget->copyFrom(rectangleTarget, pframebuffer, 0, 0);
   }

   void DesktopServerProto::sendNewClipboard(const ::scoped_string &newClipboard, BlockingGate *pblockinggate)
   {
      pblockinggate->writeUTF8(newClipboard);
   }

   void DesktopServerProto::readNewClipboard(::string &newClipboard, BlockingGate *pblockinggate)
   {
      newClipboard = pblockinggate->readUtf8();
   }

   void DesktopServerProto::sendNewPointerPos(const ::int_point pointNewPosition, unsigned char keyFlag, BlockingGate *pblockinggate)
   {
      // Send pointer pointPosition
      pblockinggate->writeUInt16(pointNewPosition.x);
      pblockinggate->writeUInt16(pointNewPosition.y);
      // Send key flags
      pblockinggate->writeUInt8(keyFlag);
   }

   void DesktopServerProto::readNewPointerPos(::int_point *pointNewPosition, unsigned char *keyFlag, BlockingGate *pblockinggate)
   {
      // Read pointer pointPosition
      pointNewPosition->x = pblockinggate->readUInt16();
      pointNewPosition->y = pblockinggate->readUInt16();
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
      auto pserverconfig = m_pconfigurator->getServerConfig();

      // Log
      pblockinggate->writeUInt32(pserverconfig->getLogLevel());

      // Polling
      pblockinggate->writeUInt32(pserverconfig->getPollingInterval());
      pblockinggate->writeUInt8(pserverconfig->getGrabTransparentWindowsFlag());

      //
      pblockinggate->writeUInt8(pserverconfig->isBlockingLocalInput());
      pblockinggate->writeUInt8(pserverconfig->isLocalInputPriorityEnabled());
      pblockinggate->writeUInt32(pserverconfig->getLocalInputPriorityTimeout());

      pblockinggate->writeUInt8(pserverconfig->isRemovingDesktopWallpaperEnabled());

      // Send video class names
      AutoLock al(pserverconfig);
      ::string_array *wndClassNames = pserverconfig->getVideoClassNames();
      ::string_array::iterator iter = wndClassNames->begin();
      size_t stringCount = wndClassNames->size();
      pblockinggate->writeUInt32((unsigned int)stringCount);
      for (; iter < wndClassNames->end(); iter++)
      {
         pblockinggate->writeUTF8((*iter));
      }
      // Send video rectanglea
      ::int_rectangle_array_base *Rects = pserverconfig->getVideoRects();
      size_t size = Rects->size();
      pblockinggate->writeUInt32((unsigned int)size);
      for (size_t i = 0; i < size; i++)
      {
         ::string s;
         RectSerializer::toString((Rects->at(i)), s);
         pblockinggate->writeUTF8(s);
      }
      // Send video recognition interval
      pblockinggate->writeUInt32(pserverconfig->getVideoRecognitionInterval());
      // Send socket timeout
      pblockinggate->writeUInt32(pserverconfig->getIdleTimeout());
   }

   void DesktopServerProto::readConfigSettings(BlockingGate *pblockinggate)
   {
      ::remoting_node::ServerConfig *pserverconfig = m_pconfigurator->getServerConfig();

      // Log
      pserverconfig->setLogLevel(pblockinggate->readUInt32());

      // Polling
      pserverconfig->setPollingInterval(pblockinggate->readUInt32());
      pserverconfig->setGrabTransparentWindowsFlag(pblockinggate->readUInt8() != 0);

      pserverconfig->blockLocalInput(pblockinggate->readUInt8() != 0);
      pserverconfig->setLocalInputPriority(pblockinggate->readUInt8() != 0);
      pserverconfig->setLocalInputPriorityTimeout(pblockinggate->readUInt32());

      pserverconfig->enableRemovingDesktopWallpaper(pblockinggate->readUInt8() != 0);

      // Receive video class names
      AutoLock al(pserverconfig);
      size_t stringCount = pblockinggate->readUInt32();

      ::string tmpString;
      for (size_t i = 0; i < stringCount; i++)
      {
         tmpString= pblockinggate->readUtf8();
         pserverconfig->getVideoClassNames()->add(tmpString);
      }
      // Receive video rectanglea
      stringCount = pblockinggate->readUInt32();

      tmpString = "";
      for (size_t i = 0; i < stringCount; i++)
      {
         tmpString = pblockinggate->readUtf8();
         pserverconfig->getVideoRects()->add(RectSerializer::toRect(tmpString));
      }

      // Receive video recognition interval
      pserverconfig->setVideoRecognitionInterval(pblockinggate->readUInt32());
      // Receive socket timeout
      pserverconfig->setIdleTimeout(pblockinggate->readUInt32());
   }


} // namespace remoting
