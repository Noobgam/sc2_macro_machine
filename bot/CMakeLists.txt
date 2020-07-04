include_directories("../include")

# Included for all examples
include_directories(SYSTEM "${PROJECT_BINARY_DIR}/generated")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/contrib/SDL-mirror/include")

file (GLOB_RECURSE all_sources "*.h" "*.cpp" "*.hpp")

add_definitions("-DSC2API")
add_definitions("-DDEBUG_LOGGING")

add_executable(main_bot ${all_sources})
target_link_libraries(main_bot sc2api sc2lib sc2utils sc2protocol)

add_executable(main_bot_debug ${all_sources})
target_compile_definitions(main_bot_debug PUBLIC -D_DEBUG)
target_link_libraries(main_bot_debug sc2api sc2lib sc2utils sc2protocol)

