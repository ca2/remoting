cmake_minimum_required(VERSION 3.16)
project(remoting_desktop)





list(APPEND library_source
   framework.cpp
   framework.h
   WinVideoRegionUpdaterImpl.h
   WinServiceDesktopFactory.h
   Poller.h
   WindowsInputBlocker.h
   WindowsScreenGrabber.cpp
   WindowsCursorShapeGrabber.h
   UserInput.h
   UpdateContainer.h
   WinDxgiOutputDuplication.cpp
   Win8DuplicationListener.h
   UpdateFilter.h
   WinDxgiAdapter.cpp
   WinAutoMapDxgiSurface.cpp
   WinDxException.cpp
   UpdateListener.cpp
   SasUserInput.h
   WinD3D11Device.cpp
   Poller.cpp
   Win32MirrorScreenDriver.h
   WinDxgiOutputDuplication.h
   WinCustomD3D11Texture2D.h
   Win8DeskDuplicationThread.cpp
   WinD3D11Texture2D.cpp
   WinDxgiAcquiredFrame.h
   UpdateKeeper.cpp
   WinServiceDesktopFactory.cpp
   ScreenGrabber.h
   Win32ScreenDriverBaseImpl.cpp
   UpdateHandler.cpp
   Win32ScreenDriverFactory.cpp
   WinDxgiOutput.h
   Win32MirrorScreenDriver.cpp
   MirrorScreenDriver.h
   WindowsClipboard.cpp
   WindowsUserInput.h
   Win8DeskDuplicationThread.h
   WinDxRecoverableException.cpp
   ScreenDriverFactory.h
   SasUserInput.cpp
   UpdateContainer.cpp
   UpdateKeeper.h
   WinDxCriticalException.cpp
   WallpaperUtil.cpp
   WinCustomD3D11Texture2D.cpp
   Win8ScreenDriver.h
   UserInput.cpp
   WinDxgiAdapter.h
   Win8ScreenDriverImpl.cpp
   UpdateSendingListener.cpp
   WinD3D11Texture2D.h
   Win32ScreenDriver.h
   Win8CursorShape.cpp
   WinDxException.h
   Win8CursorShape.h
   WinCursorShapeUtils.h
   WinDxgiAcquiredFrame.cpp
   WindowsCursorShapeGrabber.cpp
   Win8ScreenDriver.cpp
   WinDxCriticalException.h
   Win8ScreenDriverImpl.h
   UpdateSendingListener.h
   WindowsClipboard.h
   WindowsScreenGrabber.h
   WinAutoMapDxgiSurface.h
   WinDxgiDevice.cpp
   WinDxgiSurface.h
   WinDxgiDevice.h
   UpdateHandlerImpl.h
   WinD3D11Device.h
   WinDxgiOutput.cpp
   MirrorDriverClient.h
   Win32ScreenDriverFactory.h
   UpdateHandlerImpl.cpp
   WindowsInputBlocker.cpp
   UpdateHandler.h
   UpdateFilter.cpp
   MirrorScreenDriver.cpp
   Win32ScreenDriver.cpp
   WindowsUserInput.cpp
   WinDxgiOutput1.h
   MirrorDriverClient.cpp
   UpdateDetector.h
   WallpaperUtil.h
   WinDxgiOutput1.cpp
   WinDxgiSurface.cpp
   UpdateDetector.cpp
   ScreenDriver.h
   resource.h
   ScreenGrabber.cpp
   WinCursorShapeUtils.cpp
   UpdateListener.h
   Win32ScreenDriverBaseImpl.h
   WinVideoRegionUpdaterImpl.cpp
   WinDxRecoverableException.h
   InputBlocker.h
   HooksUpdateDetector.cpp
   HookUpdateTimer.h
   GrabOptimizator.h
   HookUpdateTimer.cpp
   HooksUpdateDetector.h
   HookInstaller.h
   GrabOptimizator.cpp
   InputBlocker.cpp
   HookInstaller.cpp
   DummyScreenDriver.h
   DesktopBaseImpl.h
   DesktopConfigLocal.h
   DesktopFactory.h
   DesktopClientImpl.cpp
   DesktopBaseImpl.cpp
   DesktopServerWatcher.cpp
   DummyScreenDriver.cpp
   DesktopConfigLocal.cpp
   DisplayEsc.h
   DesktopWinImpl.cpp
   DesktopServerWatcher.h
   DesktopWinImpl.h
   DesktopClientImpl.h
   Desktop.h
   CursorPositionDetector.cpp
   CursorShapeGrabber.h
   CursorShapeDetector.h
   CursorPositionDetector.h
   CopyRectDetector.cpp
   CursorShapeDetector.cpp
   CursorShapeGrabber.cpp
   ConsolePoller.h
   ConsolePoller.cpp
   CopyRectDetector.h
   ClipboardListener.h
   ClipboardListener.cpp
   ApplicationDesktopFactory.cpp
   ApplicationDesktopFactory.h
   AbnormDeskTermListener.h
   _.h
   _.cpp
)


list(APPEND library_references
   acme
)


list(APPEND library_include_directories
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_CURRENT_SOURCE_DIR}/..
   ${CMAKE_CURRENT_SOURCE_DIR}
)


add_library(${PROJECT_NAME} STATIC ${library_source})



list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${library_include_directories})



