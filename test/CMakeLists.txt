FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        b796f7d44681514f58a683a3a71ff17c94edb0c1 # release-1.13.0
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        sample_test
        sample_test.cpp)

target_link_libraries(
        sample_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(sample_test)