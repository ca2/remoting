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
#include "WallpaperUtil.h"
#include "subsystem/node/SystemException.h"

#import <AppKit/AppKit.h>


namespace remoting_macos
{

   namespace
   {

      void run_on_main_thread(void (^block)(void))
      {

         if ([NSThread isMainThread])
         {

            block();

         }
         else
         {

            dispatch_sync(dispatch_get_main_queue(), block);

         }

      }


      NSString * blank_wallpaper_path()
      {

         NSString * path = [NSTemporaryDirectory() stringByAppendingPathComponent:@"remoting_blank_wallpaper.tiff"];

         if ([[NSFileManager defaultManager] fileExistsAtPath:path])
         {

            return path;

         }

         NSBitmapImageRep * bitmap = [[NSBitmapImageRep alloc]
            initWithBitmapDataPlanes:NULL
            pixelsWide:1
            pixelsHigh:1
            bitsPerSample:8
            samplesPerPixel:4
            hasAlpha:YES
            isPlanar:NO
            colorSpaceName:NSCalibratedRGBColorSpace
            bytesPerRow:4
            bitsPerPixel:32];

         if (!bitmap)
         {

            return nil;

         }

         unsigned char * data = [bitmap bitmapData];
         data[0] = 0;
         data[1] = 0;
         data[2] = 0;
         data[3] = 255;

         NSData * tiff = [bitmap TIFFRepresentation];

         if (![tiff writeToFile:path atomically:YES])
         {

            return nil;

         }

         return path;

      }


      NSDictionary * blank_wallpaper_options()
      {

         return @{
            NSWorkspaceDesktopImageFillColorKey : [NSColor blackColor],
            NSWorkspaceDesktopImageAllowClippingKey : @NO,
            NSWorkspaceDesktopImageScalingKey : @(NSImageScaleAxesIndependently)
         };

      }


      NSString * ns_string_from_string(const ::string & str)
      {

         return [NSString stringWithUTF8String:str.c_str()];

      }

   } // namespace


   WallpaperUtil::WallpaperUtil()
   {

   }


   WallpaperUtil::~WallpaperUtil()
   {

      if (m_bWasDisabled)
      {

         try
         {

            restoreWallpaper();
            m_bWasDisabled = false;
            m_plogwriter->information("Wallpaper was successfully restored");

         }
         catch (::exception & e)
         {

            m_plogwriter->error(e.get_message());

         }

      }

   }


   void WallpaperUtil::disableWallpaper()
   {

      m_plogwriter->information("Try to disable wallpaper");

      __block bool ok = true;
      __block NSString * errorMessage = nil;
      auto pThis = this;

      run_on_main_thread(^
      {

         NSArray<NSScreen *> * screens = [NSScreen screens];
         NSString * blankPath = blank_wallpaper_path();

         if ([screens count] == 0 || !blankPath)
         {

            ok = false;
            errorMessage = @"Cannot create blank desktop wallpaper";
            return;

         }

         NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
         NSURL * blankURL = [NSURL fileURLWithPath:blankPath];
         NSDictionary * options = blank_wallpaper_options();

         pThis->m_straWallpaperUrl.clear();
         pThis->m_strBlankWallpaperPath = [blankPath UTF8String];

         for (NSScreen * screen in screens)
         {

            NSURL * url = [workspace desktopImageURLForScreen:screen];
            NSString * urlString = [url absoluteString];
            pThis->m_straWallpaperUrl.add(urlString ? [urlString UTF8String] : "");

            NSError * error = nil;
            if (![workspace setDesktopImageURL:blankURL forScreen:screen options:options error:&error])
            {

               ok = false;
               errorMessage = [error localizedDescription];
               return;

            }

         }

      });

      if (!ok)
      {

         ::string strMessage("Cannot disable desktop wallpaper");
         if (errorMessage)
         {

            strMessage += ": ";
            strMessage += [errorMessage UTF8String];

         }
         throw ::subsystem::SystemException(strMessage);

      }

   }


   void WallpaperUtil::restoreWallpaper()
   {

      m_plogwriter->information("Try to restore wallpaper");

      __block bool ok = true;
      __block NSString * errorMessage = nil;
      auto pThis = this;

      run_on_main_thread(^
      {

         NSArray<NSScreen *> * screens = [NSScreen screens];
         NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
         auto count = minimum((::collection::count) [screens count], pThis->m_straWallpaperUrl.get_count());

         for (::collection::index i = 0; i < count; ++i)
         {

            const auto & strUrl = pThis->m_straWallpaperUrl[i];

            if (strUrl.is_empty())
            {

               continue;

            }

            NSString * string = ns_string_from_string(strUrl);
            NSURL * url = [NSURL URLWithString:string];

            if (!url)
            {

               continue;

            }

            NSError * error = nil;
            if (![workspace setDesktopImageURL:url forScreen:[screens objectAtIndex:i] options:@{} error:&error])
            {

               ok = false;
               errorMessage = [error localizedDescription];
               return;

            }

         }

      });

      if (!ok)
      {

         ::string strMessage("Cannot restore desktop wallpaper");
         if (errorMessage)
         {

            strMessage += ": ";
            strMessage += [errorMessage UTF8String];

         }
         throw ::subsystem::SystemException(strMessage);

      }

   }


} // namespace remoting_macos
