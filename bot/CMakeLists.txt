include_directories("../include")

# Included for all examples
include_directories(SYSTEM "${PROJECT_BINARY_DIR}/generated")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/contrib/SDL-mirror/include")

file (GLOB_RECURSE all_sources "*.h" "*.cpp" "*.hpp")

add_definitions("-DSC2API")
add_executable(main_bot ${all_sources})
target_link_libraries(main_bot sc2api sc2lib sc2utils sc2protocol)

