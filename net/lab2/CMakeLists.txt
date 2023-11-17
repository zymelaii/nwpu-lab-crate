# portsc
set(PORTSC_SOURCE_FILES portsc.cpp)

add_executable(portsc ${PORTSC_SOURCE_FILES})
target_link_libraries(
    portsc
    PRIVATE wsock32 ws2_32
)

# depslib
set(DEPSLIB_SOURCE_FILES crc.cpp ethernet.cpp)
add_library(depslib OBJECT ${DEPSLIB_SOURCE_FILES})

# eth-client
set(ETH_CLIENT_SOURCE_FILES eth-client.cpp)

add_executable(eth-client ${ETH_CLIENT_SOURCE_FILES})
target_link_directories(
    eth-client
    PRIVATE ${CMAKE_SOURCE_DIR}/deps/lib64
)
target_include_directories(
    eth-client
    PRIVATE ${CMAKE_SOURCE_DIR}/deps/include
)
target_link_libraries(
    eth-client
    PRIVATE wsock32 ws2_32
    PRIVATE Packet wpcap
    PRIVATE depslib
)

# eth-server
set(ETH_SERVER_SOURCE_FILES eth-server.cpp)

add_executable(eth-server ${ETH_SERVER_SOURCE_FILES})
target_link_directories(
    eth-server
    PRIVATE ${CMAKE_SOURCE_DIR}/deps/lib64
)
target_include_directories(
    eth-server
    PRIVATE ${CMAKE_SOURCE_DIR}/deps/include
)
target_link_libraries(
    eth-server
    PRIVATE wsock32 ws2_32
    PRIVATE Packet wpcap
    PRIVATE depslib
)

add_custom_target(lab2 DEPENDS portsc eth-client eth-server)
