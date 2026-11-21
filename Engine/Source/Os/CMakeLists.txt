file(GLOB_RECURSE PRIVATE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(Os STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(Os
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)

if(WIN32)
    target_link_libraries(Os
        PUBLIC
            imm32
            wininet
            ws2_32
            dinput8
            dxguid
            version
    )
endif()

if(WIN32 AND WOWDECOMP_TOOL_FEATURES)
    # Tool features: the OsGui toolkit (Os/GuiToolkit.inl) uses image lists, InitCommonControlsEx and _TrackMouseEvent.
    target_link_libraries(Os PUBLIC comctl32)
endif()
