include_directories("../include")

# Included for all examples
include_directories(SYSTEM "${PROJECT_BINARY_DIR}/generated")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/contrib/SDL-mirror/include")

include_directories("../bot")

add_definitions("-DSC2API")
add_definitions("-DDEBUG_LOGGING")

file (GLOB_RECURSE all_sources_bot "../bot/*.h" "../bot/*.cpp" "../bot/*.hpp")

# not really cool, but it does what it should
list(FILTER all_sources_bot EXCLUDE REGEX ".*main.cpp$")

# to run tests locally
file (GLOB_RECURSE all_sources "*.h" "*.cpp" "*.hpp")
add_executable(test_all_debug ${all_sources} ${all_sources_bot})
target_compile_definitions(test_all_debug PUBLIC -D_DEBUG)
target_link_libraries(test_all_debug sc2api sc2lib sc2utils sc2protocol)

add_executable(test_all ${all_sources} ${all_sources_bot})
target_compile_options(test_all PUBLIC -fsanitize=address -fno-omit-frame-pointer)
set_target_properties(test_all PROPERTIES LINK_FLAGS -fsanitize=address)

