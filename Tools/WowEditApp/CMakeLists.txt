# WowEdit64 / WowEdit32: the world editor application (WowEdit/Source/App/WowEditApp.h). Built only with
# WOWDECOMP_TOOL_FEATURES: the client sources and libraries of WowClient (Tools/WowClient) plus WowEditAppMain.cpp,
# which turns the editor on before the client starts.
file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/WoW/Source/*.cpp")

if (WOWDECOMP_ARCH_64)
    file(GLOB CLIENT_PLATFORM_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/WoW/Source/Win64/*.cpp")
    set(WOWEDITAPP_OUTPUT_NAME WowEdit64)
else ()
    file(GLOB CLIENT_PLATFORM_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/WoW/Source/Win32/*.cpp")
    set(WOWEDITAPP_OUTPUT_NAME WowEdit32)
endif ()

add_executable(WowEditApp WIN32 ${CLIENT_SOURCES} ${CLIENT_PLATFORM_SOURCES} WowEditAppMain.cpp)

set_target_properties(WowEditApp PROPERTIES
    OUTPUT_NAME ${WOWEDITAPP_OUTPUT_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Win
)

foreach (config ${CMAKE_CONFIGURATION_TYPES} ${CMAKE_BUILD_TYPE})
    string(TOUPPER ${config} config)
    set_target_properties(WowEditApp PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${config} ${CMAKE_SOURCE_DIR}/bin/Win)
endforeach ()

target_include_directories(WowEditApp
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

target_link_options(WowEditApp PRIVATE "/INCREMENTAL:NO" "/ENTRY:start")

# The same libraries as WowClient (WowEdit among them).
get_target_property(WOWCLIENT_LIBRARIES WowClient LINK_LIBRARIES)
target_link_libraries(WowEditApp PRIVATE ${WOWCLIENT_LIBRARIES})
