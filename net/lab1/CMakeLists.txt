aux_source_directory(. SOURCE_FILES)

add_executable(lab1 ${SOURCE_FILES})
target_link_libraries(lab1 PRIVATE wsock32 ws2_32)
