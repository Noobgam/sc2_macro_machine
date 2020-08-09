include_directories("../cpp_sc2/include")
include_directories("../bot/include")
include_directories("./")

# Included for all examples
include_directories(SYSTEM "${PROJECT_BINARY_DIR}/cpp_sc2/generated")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/cpp_sc2/contrib/SDL-mirror/include")

include_directories("../bot")

# Build with c++17 support.
set(CMAKE_CXX_STANDARD 17)

add_definitions("-DSC2API")
add_definitions("-DDEBUG_LOGGING")
add_definitions("-D_STATIC_MAP_CALCULATOR")

file (GLOB_RECURSE all_sources_bot "../bot/*.h" "../bot/*.cpp" "../bot/*.hpp")

# not really cool, but it does what it should
list(FILTER all_sources_bot EXCLUDE REGEX ".*main.cpp$")

file (GLOB_RECURSE all_sources "*.h" "*.cpp" "*.hpp")
add_executable(static_meta_calculator ${all_sources} ${all_sources_bot})
target_link_libraries(static_meta_calculator sc2api sc2lib sc2utils sc2protocol)

