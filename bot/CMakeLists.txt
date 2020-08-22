
include_directories("../include")

# Included for all examples
include_directories(SYSTEM "${PROJECT_BINARY_DIR}/generated")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/contrib/SDL-mirror/include")


include_directories("./")

add_definitions("-DSC2API")
add_definitions("-DDEBUG_LOGGING")

file (GLOB_RECURSE all_sources "*.h" "*.cpp" "*.hpp")
add_executable(main_bot ${all_sources})
target_link_libraries(main_bot sc2api sc2lib sc2utils sc2protocol)
target_compile_options(main_bot PUBLIC -fsanitize=address -fno-omit-frame-pointer)
set_target_properties(main_bot PROPERTIES LINK_FLAGS -fsanitize=address)
target_link_libraries(main_bot sc2api sc2lib sc2utils sc2protocol -static-libgcc -static-libstdc++ -lm -lc -lgcc)

add_executable(main_bot_debug ${all_sources})
target_compile_definitions(main_bot_debug PUBLIC -D_DEBUG)
target_link_libraries(main_bot_debug sc2api sc2lib sc2utils sc2protocol -static-libgcc -static-libstdc++)
