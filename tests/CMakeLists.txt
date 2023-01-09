set(BINARY ${CMAKE_PROJECT_NAME}_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})
target_include_directories(${BINARY} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_compile_options(${BINARY} PRIVATE
        -Wall
        -Werror
        -Wextra
        #-Wpedantic # cannot use pedantic due to GNU specific Statement Expressions
        )

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest)
