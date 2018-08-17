cmake_minimum_required(VERSION 2.8.4)
include(ExternalProject)
include(ProcessorCount)

option(DEBUG "If debug build" OFF)

ProcessorCount(N)
if(NOT N EQUAL 0)
    set(X264_BUILD_PARALLEL -j${N})
endif()

list(APPEND CONFIGURE_OPTIONS --enable-static --enable-pic
    --disable-cli --prefix=${CMAKE_CURRENT_SOURCE_DIR}/install)
if (${DEBUG})
  list(APPEND CONFIGURE_OPTIONS --enable-debug --disable-asm)
else (${DEBUG})
  list(APPEND CONFIGURE_OPTIONS --enable-strip --extra-cflags=-march=native)
endif(${DEBUG})

ExternalProject_Add(x264_local
    URL http://download.videolan.org/pub/videolan/x264/snapshots/last_stable_x264.tar.bz2
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./configure ${CONFIGURE_OPTIONS}
    BUILD_COMMAND make ${X264_BUILD_PARALLEL}
    INSTALL_COMMAND make install
    )
