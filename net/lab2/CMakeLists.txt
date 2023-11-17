# depslib
set(DEPSLIB_SOURCE_FILES crc.cpp ethernet.cpp)

add_library(depslib OBJECT ${DEPSLIB_SOURCE_FILES})
target_include_directories(
    depslib
    PUBLIC ${CMAKE_SOURCE_DIR}/deps/include
)
target_link_directories(
    depslib
    PUBLIC ${CMAKE_SOURCE_DIR}/deps/lib64
)
target_link_libraries(
    depslib
    PRIVATE Packet wpcap
)

# portsc
add_executable(portsc portsc.cpp)
target_link_libraries(
    portsc
    PRIVATE wsock32 ws2_32
)

# eth-client
add_executable(eth-client eth-client.cpp)
target_link_libraries(eth-client PRIVATE depslib)

# eth-server
add_executable(eth-server eth-server.cpp)
target_link_libraries(eth-server PRIVATE depslib)

add_custom_target(lab2 DEPENDS portsc eth-client eth-server)
