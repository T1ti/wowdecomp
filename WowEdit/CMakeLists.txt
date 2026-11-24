# WowEdit: the world editor's side of the engine (WowEdit/Source/WowEdit.h). Built only with WOWDECOMP_TOOL_FEATURES;
# linked only into the standalone editor executable.
file(GLOB_RECURSE PRIVATE_SOURCES CONFIGURE_DEPENDS "Source/*.cpp")

add_library(WowEdit STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(WowEdit
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)
