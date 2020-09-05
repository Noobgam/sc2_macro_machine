file(GLOB proto_files
        "protos/*.proto"
)
message(${proto_files})
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/bot-generated/bot)
foreach(proto ${proto_files})
    get_filename_component(proto_name ${proto} NAME_WE)
    list(APPEND proto_src ${PROJECT_BINARY_DIR}/bot-generated/bot/${proto_name}.pb.h ${PROJECT_BINARY_DIR}/bot-generated/bot/${proto_name}.pb.cc)
endforeach()

add_library(bot-protos ${proto_src} ${proto_files})

foreach (proto ${proto_files})
    get_filename_component(proto_name ${proto} NAME)
    get_filename_component(proto_name_we ${proto} NAME_WE)
    set(outputCC ${PROJECT_BINARY_DIR}/bot-generated/bot/${proto_name_we}.pb.cc)
    set(outputH ${PROJECT_BINARY_DIR}/bot-generated/bot/${proto_name_we}.pb.h)

    message(
            DEPENDS protoc
            COMMAND ${PROJECT_BINARY_DIR}/bin/protoc -I=${CMAKE_CURRENT_SOURCE_DIR}/protos/ --cpp_out=${PROJECT_BINARY_DIR}/bot-generated ${proto}
            OUTPUT ${outputCC} ${outputH}
    )
    add_custom_command(
            DEPENDS protoc
            COMMAND ${PROJECT_BINARY_DIR}/bin/protoc -I=${CMAKE_CURRENT_SOURCE_DIR}/protos/ --cpp_out=${PROJECT_BINARY_DIR}/bot-generated/bot/ ${proto}
            OUTPUT ${outputCC} ${outputH}
    )
endforeach()

add_dependencies(bot-protos protoc)
target_link_libraries(bot-protos libprotobuf)

include_directories("../include")

# Included for all examples
include_directories(SYSTEM "${PROJECT_BINARY_DIR}/bot-generated")
include_directories(SYSTEM "${PROJECT_BINARY_DIR}/generated")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/contrib/SDL-mirror/include")


include_directories("./")

add_definitions("-DSC2API")
add_definitions("-DDEBUG_LOGGING")

file (GLOB_RECURSE all_sources "*.h" "*.cpp" "*.hpp")
add_executable(main_bot ${all_sources})
target_link_libraries(main_bot bot-protos sc2api sc2lib sc2utils sc2protocol -static-libgcc -static-libstdc++ -lm -lc -lgcc)

add_executable(main_bot_debug ${all_sources})
target_compile_definitions(main_bot_debug PUBLIC -D_DEBUG)
target_link_libraries(main_bot_debug bot-protos sc2api sc2lib sc2utils sc2protocol -static-libgcc -static-libstdc++)

add_executable(main_bot_asan ${all_sources})
target_link_libraries(main_bot_asan bot-protos sc2api sc2lib sc2utils sc2protocol)
target_compile_options(main_bot_asan PUBLIC -fsanitize=address -fno-omit-frame-pointer)
set_target_properties(main_bot_asan PROPERTIES LINK_FLAGS -fsanitize=address)
