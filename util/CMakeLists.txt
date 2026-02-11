cmake_minimum_required(VERSION 3.16)
project(remoting_util)




list(APPEND library_source
   _.cpp
   _.h
   AnEventListener.h
   AnsiStringStorage.cpp
   AnsiStringStorage.h
   BrokenHandleException.cpp
   BrokenHandleException.h
   CharDefs.h
   Command.cpp
   CommandLine.h
   Command.h
   CommandLineArgs.h
   CommandLine.cpp
   CommandLineFormatHelp.cpp
   CommandLineFormatHelp.h
   CommandLineArgs.cpp
   CommonHeader.h
   CommandLineFormatException.h
   CommandLineFormatException.cpp
   ZLibBase.cpp
   ZLibBase.h
   ZlibException.cpp
   ZlibException.h
   winhdr.h
   VncPassCrypt.h
   VncPassCrypt.cpp
   Utf8StringStorage.h
   Utf8StringStorage.cpp
   UnicodeStringStorage.h
   UnicodeStringStorage.cpp
   Unicode.h
   Unicode.cpp
   StringVector.h
   StringTable.h
   StringTable.cpp
   StringStorage.h
   StringStorage.cpp
   StringParser.h
   StringParser.cpp
   Singleton.h
   ResourceLoader.h
   ResourceLoader.cpp
   MemUsage.h
   MemUsage.cpp
   md5.h
   md5.cpp
   MacroCommand.cpp
   ListenerContainer.h
   MacroCommand.h
   Keymap.h
   Keymap.cpp
   KeyContainer.h
   KeyContainer.cpp
   inttypes.h
   Inflater.h
   Inflater.cpp
   GetCPUtime.cpp
   GetCPUtime.h
   framework.h
   framework.cpp
   Exception.h
   DesCrypt.h
   DesCrypt.cpp
   DemandTimer.h
   Exception.cpp
   DemandTimer.cpp
   Deflater.h
   Deflater.cpp
   DateTime.h
   DateTime.cpp
   framework.cpp
   framework.h
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

