set(SOURCES
    src/main.c
    src/eventHandler.c
    src/eventHandlerWin.c
    src/eventQueue.c
    src/jsonHandler.c
    lib/cJSON.c
)

#if(APPLE)
#    list(APPEND SOURCES src/eventHandlerMac.c)
#elseif(UNIX)
#    list(APPEND SOURCES src/eventHandlerLinux.c)
#endif()

add_executable(keyboard-relay ${SOURCES})

target_include_directories(keyboard-relay PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/header
)