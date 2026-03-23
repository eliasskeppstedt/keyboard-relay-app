add_executable(keyboard-relay)

# Basfiler (alltid med)
target_sources(keyboard-relay PRIVATE
    src/main.c
    src/eventHandler.c
    src/eventQueue.c
    src/jsonHandler.c
    lib/cJSON.c
)

if(WIN32)
    target_sources(keyboard-relay PRIVATE
        src/eventHandlerWin.c
    )
#elseif(APPLE)
#    target_sources(keyboard-relay PRIVATE
#        src/eventHandlerMac.c
#    )
#elseif(UNIX)
#    target_sources(keyboard-relay PRIVATE
#        src/eventHandlerLinux.c
#    )
endif()

if(MSVC)
    target_compile_options(keyboard-relay PRIVATE /W4)

    set_property(TARGET keyboard-relay PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
else()
    target_compile_options(keyboard-relay PRIVATE -Wall -Wextra)
endif()

target_include_directories(keyboard-relay PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/header
)