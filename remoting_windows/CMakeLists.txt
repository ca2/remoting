cmake_minimum_required(VERSION 3.16)
project(remoting_macos)





list(APPEND library_source

   _.cpp
   _.h
   framework.cpp
   framework.h
)


if(${WIN32})

   list(APPEND library_source

      _common_header.h
      desktop/DesktopFactory.cpp
      desktop/DesktopFactory.h
      desktop/DesktopClientImpl.cpp
      desktop/DesktopClientImpl.h
   desktop/DesktopWinImpl.cpp
   desktop/DesktopWinImpl.h
   desktop/MirrorDriverClient.cpp
   desktop/MirrorDriverClient.h
   desktop/MirrorScreenDriver.cpp
   desktop/MirrorScreenDriver.h
   desktop/Win8CursorShape.cpp
   desktop/Win8CursorShape.h
   desktop/Win8DeskDuplicationThread.cpp
   desktop/Win8DeskDuplicationThread.h
   desktop/Win8DuplicationListener.h
   desktop/Win8ScreenDriver.cpp
   desktop/Win8ScreenDriver.h
   desktop/Win8ScreenDriverImpl.cpp
   desktop/Win8ScreenDriverImpl.h
   desktop/Win32MirrorScreenDriver.cpp
   desktop/Win32MirrorScreenDriver.h
   desktop/Win32ScreenDriver.cpp
   desktop/Win32ScreenDriver.h
   desktop/Win32ScreenDriverBaseImpl.cpp
   desktop/Win32ScreenDriverBaseImpl.h
   desktop/Win32ScreenDriverFactory.cpp
   desktop/Win32ScreenDriverFactory.h
   desktop/WinAutoMapDxgiSurface.cpp
   desktop/WinAutoMapDxgiSurface.h
   desktop/WinCursorShapeUtils.cpp
   desktop/WinCursorShapeUtils.h
   desktop/WinCustomD3D11Texture2D.cpp
   desktop/WinCustomD3D11Texture2D.h
   desktop/WinD3D11Device.cpp
   desktop/WinD3D11Device.h
   desktop/WinD3D11Texture2D.cpp
   desktop/WinD3D11Texture2D.h
   desktop/WindowsCursorShapeGrabber.cpp
   desktop/WindowsCursorShapeGrabber.h
   desktop/WindowsInputBlocker.cpp
   desktop/WindowsInputBlocker.h
   desktop/WindowsScreenGrabber.cpp
   desktop/WindowsScreenGrabber.h
   desktop/WindowsUserInput.cpp
   desktop/WindowsUserInput.h
   desktop/WinDxCriticalException.cpp
   desktop/WinDxCriticalException.h
   desktop/WinDxException.cpp
   desktop/WinDxException.h
   desktop/WinDxgiAcquiredFrame.cpp
   desktop/WinDxgiAcquiredFrame.h
   desktop/WinDxgiAdapter.cpp
   desktop/WinDxgiAdapter.h
   desktop/WinDxgiDevice.cpp
   desktop/WinDxgiDevice.h
   desktop/WinDxgiOutput.cpp
   desktop/WinDxgiOutput.h
   desktop/WinDxgiOutput1.cpp
   desktop/WinDxgiOutput1.h
   desktop/WinDxgiOutputDuplication.cpp
   desktop/WinDxgiOutputDuplication.h
   desktop/WinDxgiSurface.cpp
   desktop/WinDxgiSurface.h
   desktop/WinDxRecoverableException.cpp
   desktop/WinDxRecoverableException.h
   #desktop/WinServiceDesktopFactory.cpp
   #desktop/WinServiceDesktopFactory.h
   desktop/WinVideoRegionUpdaterImpl.cpp
   desktop/WinVideoRegionUpdaterImpl.h
_factory.cpp
   )
endif()

list(APPEND library_references
   acme
   subsystem
   ${default_subsystem}
   apex
   innate_subsystem
   remoting
)


list(APPEND library_include_directories
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_CURRENT_SOURCE_DIR}/..
   ${CMAKE_CURRENT_SOURCE_DIR}
)


add_library(${PROJECT_NAME} SHARED ${library_source})

#target_precompile_headers(${PROJECT_NAME} PRIVATE framework.h)



if (NOT ${NO_PRECOMPILED_HEADER})

   target_precompile_headers(${PROJECT_NAME} PRIVATE framework.h)
endif ()


set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)


if(${WIN32})


   target_compile_options(${PROJECT_NAME} PRIVATE "/MD$<$<CONFIG:Debug>:d>")
   target_link_options(${PROJECT_NAME} PRIVATE "/DEFAULTLIB:msvcrt$<$<CONFIG:Debug>:d>")
   target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
   target_link_libraries(${PROJECT_NAME} PRIVATE ${library_references} ${global_library_references})


elseif (${LINUX} OR ${__BSD__} OR ${ANDROID})


   target_link_libraries(${PROJECT_NAME} PRIVATE ${library_references} ${global_library_references})
   target_link_options(${PROJECT_NAME} PRIVATE -Wl,-z,defs)
   # target_link_options(static_${PROJECT_NAME} PRIVATE --whole-archive)

   message(STATUS "(5) PKGCONFIG_LIBRARIES is " ${${PROJECT_NAME}_PKGCONFIG_LIBRARIES})


endif ()


list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${library_include_directories})



target_compile_definitions(${PROJECT_NAME} PRIVATE _${PROJECT_NAME}_project)



