cmake_minimum_required(VERSION 3.21)

file(GLOB_RECURSE test_files *.cpp *.h *.hpp)

add_executable(runUnitTests ${test_files})

# Link against the Catch2 testing library AND the parent project
target_link_libraries(runUnitTests PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})

include(CTest)
include(Catch)
catch_discover_tests(runUnitTests)