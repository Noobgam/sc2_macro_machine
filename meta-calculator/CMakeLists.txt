include_directories("../include")

# Included for all examples
include_directories(SYSTEM "${PROJECT_BINARY_DIR}/generated")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/contrib/SDL-mirror/include")

include_directories("../bot")

add_definitions("-DSC2API")
add_definitions("-DDEBUG_LOGGING")

file (GLOB_RECURSE all_sources "*.h" "*.cpp" "*.hpp")
add_executable(meta_calculator ${all_sources})
target_link_libraries(meta_calculator sc2api sc2lib sc2utils sc2protocol)

