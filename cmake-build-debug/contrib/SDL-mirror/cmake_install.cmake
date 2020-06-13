# Install script for directory: C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/cpp-sc2")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/shess/CLionProjects/cpp-sc2/cmake-build-debug/bin/libSDL2.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/shess/CLionProjects/cpp-sc2/cmake-build-debug/bin/libSDL2main.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_assert.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_atomic.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_audio.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_bits.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_blendmode.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_clipboard.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_config_android.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_config_iphoneos.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_config_macosx.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_config_minimal.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_config_pandora.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_config_psp.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_config_windows.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_config_winrt.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_config_wiz.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_copying.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_cpuinfo.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_egl.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_endian.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_error.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_events.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_filesystem.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_gamecontroller.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_gesture.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_haptic.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_hints.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_joystick.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_keyboard.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_keycode.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_loadso.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_log.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_main.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_messagebox.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_mouse.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_mutex.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_name.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_opengl.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_opengl_glext.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_opengles.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_opengles2.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_opengles2_gl2.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_opengles2_gl2ext.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_opengles2_gl2platform.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_opengles2_khrplatform.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_pixels.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_platform.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_power.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_quit.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_rect.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_render.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_revision.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_rwops.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_scancode.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_shape.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_stdinc.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_surface.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_system.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_syswm.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test_assert.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test_common.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test_compare.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test_crc32.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test_font.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test_fuzzer.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test_harness.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test_images.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test_log.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test_md5.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_test_random.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_thread.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_timer.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_touch.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_types.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_version.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/SDL_video.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/begin_code.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/contrib/SDL-mirror/include/close_code.h"
    "C:/Users/shess/CLionProjects/cpp-sc2/cmake-build-debug/contrib/SDL-mirror/include/SDL_config.h"
    )
endif()

