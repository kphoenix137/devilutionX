#General compilation options
set(ASAN OFF)
set(UBSAN OFF)
set(BUILD_TESTING OFF)
set(DEVILUTIONX_SYSTEM_LIBSODIUM OFF)
set(DEVILUTIONX_SYSTEM_LIBFMT OFF)
set(DEVILUTIONX_STATIC_LIBSODIUM ON)
set(DEVILUTIONX_STATIC_LIBFMT ON)
set(DISABLE_ZERO_TIER ON)
set(LIBMPQ_FILE_BUFFER_SIZE 32768)
set(USE_SDL1 ON)
set(PREFILL_PLAYER_NAME ON)
set(NOEXIT ON)

# 3DS libraries and compile definitions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/ctr")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/ctr/modules")
find_package(CITRO3D REQUIRED)

list(APPEND DEVILUTIONX_PLATFORM_SUBDIRECTORIES platform/ctr)
list(APPEND DEVILUTIONX_PLATFORM_LINK_LIBRARIES libdevilutionx_ctr)
list(APPEND DEVILUTIONX_PLATFORM_COMPILE_DEFINITIONS __3DS__)

#SDL video mode parameters
set(SDL1_VIDEO_MODE_FLAGS SDL_DOUBLEBUF|SDL_HWSURFACE)
set(SDL1_FORCE_SVID_VIDEO_MODE ON)
set(SDL1_VIDEO_MODE_BPP 8)
set(DEFAULT_WIDTH 800)
set(DEFAULT_HEIGHT 480)

#SDL Joystick axis mapping (circle-pad/C-stick)
set(JOY_AXIS_LEFTX 0)
set(JOY_AXIS_LEFTY 1)
set(JOY_AXIS_RIGHTX 3)
set(JOY_AXIS_RIGHTY 4)
#SDL Joystick hat mapping (D-pad)
set(JOY_HAT_DPAD_UP_HAT 0)
set(JOY_HAT_DPAD_RIGHT_HAT 0)
set(JOY_HAT_DPAD_DOWN_HAT 0)
set(JOY_HAT_DPAD_LEFT_HAT 0)
set(JOY_HAT_DPAD_UP 1)
set(JOY_HAT_DPAD_RIGHT 2)
set(JOY_HAT_DPAD_DOWN 4)
set(JOY_HAT_DPAD_LEFT 8)
#SDL Joystick button mapping (A / B and X / Y inverted)
set(JOY_BUTTON_A 2)
set(JOY_BUTTON_B 1)
set(JOY_BUTTON_X 4)
set(JOY_BUTTON_Y 3)
set(JOY_BUTTON_LEFTSHOULDER 5)
set(JOY_BUTTON_RIGHTSHOULDER 6)
set(JOY_BUTTON_BACK 7)
set(JOY_BUTTON_START 0)
set(JOY_BUTTON_TRIGGERLEFT 8)
set(JOY_BUTTON_TRIGGERRIGHT 9)
