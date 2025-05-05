enable_testing()

add_executable(tests
    test_time.cpp
    Time.cpp
)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tests gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)

