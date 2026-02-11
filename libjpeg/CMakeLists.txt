cmake_minimum_required(VERSION 3.16)
project(remoting_libjpeg C)


list(APPEND library_source
   jctrans.c
   jdatasrc.c
   jcmainct.c
   jdapimin.c
   jcapimin.c
   jcdctmgr.c
   jccolor.c
   jcarith.c
   jcapistd.c
   jcmarker.c
   _.cpp
   jdapistd.c
   jcsample.c
   jdarith.c
   jcomapi.c
   jdcolor.c
   jdcoefct.c
   jaricom.c
   jcparam.c
   _.h
   jcinit.c
   jcmaster.c
   jdatadst.c
   jchuff.c
   jccoefct.c
   jcprepct.c
   jdct.h
   jversion.h
   jfdctfst.c
   jdsample.c
   jdhuff.c
   jfdctflt.c
   jquant1.c
   jmorecfg.h
   jdmerge.c
   jidctint.c
   jmemnobs.c
   jquant2.c
   jdpostct.c
   jdmaster.c
   jddctmgr.c
   jdtrans.c
   jdinput.c
   jmemmgr.c
   jutils.c
   jidctflt.c
   jdmainct.c
   jerror.h
   jerror.c
   jpegint.h
   jmemsys.h
   jinclude.h
   jfdctint.c
   jdmarker.c
   jidctfst.c
   jpeglib.h
   resource.h
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

