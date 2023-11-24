set(SOURCE_FILES broadcast.cpp)

add_executable(broadcast ${SOURCE_FILES})
target_link_libraries(
    broadcast
    PRIVATE wsock32 ws2_32
)

add_custom_target(lab3 DEPENDS broadcast)
