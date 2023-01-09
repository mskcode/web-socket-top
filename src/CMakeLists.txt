set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

set(SOURCES ${SOURCES})
set(LINK_LIBRARY_TARGETS fmt)

add_executable(${BINARY} ${SOURCES})
target_compile_options(${BINARY} PRIVATE
        -Wall
        -Werror
        -Wextra
        #-Wpedantic # cannot use pedantic due to GNU specific Statement Expressions
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Release>:-O2>
        )
target_link_libraries(${BINARY} PUBLIC ${LINK_LIBRARY_TARGETS})

add_library(${BINARY}_lib STATIC ${SOURCES})
target_compile_options(${BINARY}_lib PRIVATE
        -Wall
        -Werror
        -Wextra
        #-Wpedantic # cannot use pedantic due to GNU specific Statement Expressions
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Release>:-O2>
        )
target_link_libraries(${BINARY}_lib PUBLIC ${LINK_LIBRARY_TARGETS})

#add_custom_command(TARGET ${BINARY} PRE_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${BINARY}>/resources)
