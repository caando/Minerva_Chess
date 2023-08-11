FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG b796f7d44681514f58a683a3a71ff17c94edb0c1 # release-1.13.0
)

FetchContent_MakeAvailable(googletest)

enable_testing()

set(
        source_files
        ${CMAKE_SOURCE_DIR}/src/nnue/nnue.h ${CMAKE_SOURCE_DIR}/src/nnue/nnue.cpp
        ${CMAKE_SOURCE_DIR}/src/nnue/misc.h ${CMAKE_SOURCE_DIR}/src/nnue/misc.cpp
        ${CMAKE_SOURCE_DIR}/src/types.h
        ${CMAKE_SOURCE_DIR}/src/magic_constants.h
        ${CMAKE_SOURCE_DIR}/src/attack.h ${CMAKE_SOURCE_DIR}/src/attack.cpp
        ${CMAKE_SOURCE_DIR}/src/perft.h ${CMAKE_SOURCE_DIR}/src/perft.cpp
        ${CMAKE_SOURCE_DIR}/src/movegen.h
        ${CMAKE_SOURCE_DIR}/src/eval.h
        ${CMAKE_SOURCE_DIR}/src/search.h ${CMAKE_SOURCE_DIR}/src/search.cpp
        ${CMAKE_SOURCE_DIR}/src/transposition.h ${CMAKE_SOURCE_DIR}/src/transposition.cpp
        ${CMAKE_SOURCE_DIR}/src/uci.h ${CMAKE_SOURCE_DIR}/src/uci.cpp
        ${CMAKE_SOURCE_DIR}/src/util.h
        ${CMAKE_SOURCE_DIR}/src/variables.h ${CMAKE_SOURCE_DIR}/src/variables.cpp
        ${CMAKE_SOURCE_DIR}/src/zobrist.h ${CMAKE_SOURCE_DIR}/src/zobrist.cpp
        ${CMAKE_SOURCE_DIR}/src/communication.h ${CMAKE_SOURCE_DIR}/src/communication.cpp
        ${CMAKE_SOURCE_DIR}/src/debug.h ${CMAKE_SOURCE_DIR}/src/debug.cpp
)

add_executable(
        sample_test
        sample_test.cpp
)

target_link_libraries(
        sample_test
        GTest::gtest_main
)

add_executable(
        movegen_test
        movegen_test.cpp
        ${source_files}
)

target_link_libraries(
        movegen_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(sample_test)