cmake_minimum_required(VERSION 3.16)
project(remoting_gui)


list(APPEND library_source
   framework.cpp
   framework.h
   Menu.cpp
   Menu.h
   MessageWindow.cpp
   MessageWindow.h
   NotifyIcon.cpp
   NotifyIcon.h
   NotifyIconWindow.cpp
   NotifyIconWindow.h
   PaintWindow.cpp
   PaintWindow.h
   ProgressBar.cpp
   ProgressBar.h
   resource.h
   ScrollBar.cpp
   ScrollBar.h
   SpinControl.cpp
   SpinControl.h
   StringFilter.cpp
   StringFilter.h
   Tab.cpp
   Tab.h
   TabContainer.cpp
   TabContainer.h
   TabControl.cpp
   TabControl.h
   TextBox.cpp
   TextBox.h
   ThemeLib.cpp
   ThemeLib.h
   ToolBar.cpp
   ToolBar.h
   Tooltip.cpp
   Tooltip.h
   Trackbar.cpp
   Trackbar.h
   WindowFinder.cpp
   WindowFinder.h
   WindowMessageHandler.h
   WindowProcHolder.cpp
   WindowProcHolder.h
   ListView.h
   ImagedButton.cpp
   ListBox.cpp
   ImagedButton.h
   ListView.cpp
   ListBox.h
   FilteredTextBox.h
   DibFrameBuffer.h
   CommonControlsEx.cpp
   DibFrameBuffer.cpp
   CommonControlsEx.h
   ComboBox.cpp
   Control.h
   Control.cpp
   FilteredTextBox.cpp
   ComboBox.h
   CheckBox.h
   CheckBox.cpp
   BaseDialog.cpp
   BalloonTip.h
   BaseWindow.cpp
   BaseWindow.h
   BaseDialog.h
   BalloonTip.cpp
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






#target_link_directories(${PROJECT_NAME} PRIVATE ${library_link_directories})

target_precompile_headers(${PROJECT_NAME} PRIVATE framework.h)
#target_precompile_headers(static_${PROJECT_NAME} PRIVATE framework.h)

#set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)


if(${WIN32})
   target_compile_options(${PROJECT_NAME} PRIVATE "/MD$<$<CONFIG:Debug>:d>")
   target_compile_options(${PROJECT_NAME} PRIVATE "/bigobj")
   target_link_options(${PROJECT_NAME} PRIVATE "/DEFAULTLIB:msvcrt$<$<CONFIG:Debug>:d>")
   target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
   target_link_libraries(${PROJECT_NAME} PRIVATE ${library_references} ${global_library_references})

elseif(${LINUX} OR ${__BSD__} OR ${ANDROID})
   target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fpermissive  ${EXTRA_CXX_TARGET_COMPILER_OPTIONS}>)
   #target_link_options(${PROJECT_NAME} PRIVATE -Wl,-rpath=$ORIGIN)
   #target_link_libraries(${PROJECT_NAME} PRIVATE ${library_references} ${global_library_references})
   #target_link_options(${PROJECT_NAME} PRIVATE -Wl,-z,defs)

endif ()

list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${library_include_directories})

