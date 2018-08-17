cmake_minimum_required(VERSION 2.8.4)
include(ExternalProject)

include(ProcessorCount)

ProcessorCount(N)
if(NOT N EQUAL 0)
    set(FFMPEG_BUILD_PARALLEL -j${N})
endif()

#project(ffmpeg-local)

option(DEBUG "If debug build" OFF)

set(FFMPEG_EXTRA_CFLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/../x264/install/include ")

if (${DEBUG})
  message("Debug build of ffmpeg")
  list(APPEND FFMPEG_OPTIM_OPTIONS --enable-debug --disable-optimizations --disable-x86asm --disable-asm --disable-stripping)
  set(FFMPEG_EXTRA_CFLAGS "${FFMPEG_EXTRA_CFLAGS} -O0 -g -ggdb ")
else (${DEBUG})
  message("Optimized build of ffmpeg")
  list(APPEND FFMPEG_OPTIM_OPTIONS --disable-debug --enable-optimizations --enable-x86asm --enable-stripping)
  set(FFMPEG_EXTRA_CFLAGS "${FFMPEG_EXTRA_CFLAGS} -march=native")
endif (${DEBUG})

set(FFMPEG_EXTRA_LIBS
    "${CMAKE_CURRENT_SOURCE_DIR}/../x264/install/lib/libx264.a -ldl")
set(FFMPEG_EXTRA_LDFLAGS " -L${CMAKE_CURRENT_SOURCE_DIR}/../x264/install/lib -ldl")

list (APPEND FFMPEG_CONFIGURE_OPTIONS
--enable-ffmpeg
--disable-protocols
--enable-protocol=file
--enable-protocol=pipe
--enable-protocol=rtp
--enable-protocol=udp
--disable-avdevice
--disable-indevs
--disable-outdevs
--disable-filters
--enable-filter=hqdn3d
--enable-filter=scale
--enable-filter=settb
--enable-filter=setpts
--enable-filter=fps
--disable-muxers
--enable-muxer=flv
--enable-muxer=mp4
--enable-muxer=webm
--enable-muxer=mpegts
--disable-demuxers
--enable-demuxer=image2
--enable-demuxer=concat
--enable-demuxer=flv
--enable-demuxer=mov
--enable-demuxer=rtp
--enable-demuxer=rtsp
--enable-demuxer=matroska
--enable-demuxer=mpegts
--disable-encoders
--enable-encoder=libx264
--enable-encoder=libvpx_vp8
--disable-decoders
--enable-decoder=mjpeg
--enable-decoder=h264
--enable-decoder=vp8
--disable-parsers
--enable-parser=h264
--enable-parser=mpeg4video
--enable-parser=mjpeg
--enable-static
--disable-bsfs
--enable-bsf=h264_mp4toannexb
--disable-postproc
--disable-doc
--disable-libgsm
--disable-zlib
--enable-libx264
--enable-libvpx
--enable-gpl
--extra-libs=${FFMPEG_EXTRA_LIBS}
--extra-cflags=${FFMPEG_EXTRA_CFLAGS}
--extra-ldflags=${FFMPEG_EXTRA_LDFLAGS}
${FFMPEG_OPTIM_OPTIONS}
--prefix=${CMAKE_CURRENT_SOURCE_DIR}/install
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../x264 ${CMAKE_CURRENT_SOURCE_DIR}/../x264)

ExternalProject_Add(ffmpeg-local
  DEPENDS x264_local
  URL https://ffmpeg.org/releases/ffmpeg-4.0.2.tar.xz
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ./configure ${FFMPEG_CONFIGURE_OPTIONS}
  BUILD_COMMAND make ${FFMPEG_BUILD_PARALLEL}
  )
