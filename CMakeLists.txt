cmake_minimum_required(VERSION 3.21)

# Sanity check
if(NOT WIN32)
    message(FATAL_ERROR "We can build this project only on Windows, sorry! :(")
endif()

# Check if CMAKE_TOOLCHAIN_FILE is set
if(DEFINED CMAKE_TOOLCHAIN_FILE)
  # Check if the file exists
  if(EXISTS ${CMAKE_TOOLCHAIN_FILE})
    message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
  else()
    message(FATAL_ERROR "Toolchain file not found at: ${CMAKE_TOOLCHAIN_FILE}")
  endif()
else()
  # If CMAKE_TOOLCHAIN_FILE is not set, you can set a default value
  unset(CMAKE_TOOLCHAIN_FILE)
  message(STATUS "CMAKE_TOOLCHAIN_FILE not set, using default: ${CMAKE_TOOLCHAIN_FILE}")
endif()

set(PROJECT_NAME "skse_plugin_template")

# Set your project name. This will be the name of your SKSE .dll file.
project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

include(GNUInstallDirs)
 
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
 
# Set up and configure VCPKG
find_program(GIT_EXECUTABLE git)
if(NOT GIT_EXECUTABLE)
    message(FATAL_ERROR "git is not found on your system. Please install it and run the configuration again.")
endif()

if(DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
else()
    set(VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")
endif()

message("Using vcpkg from path: ${VCPKG_ROOT}")

if(NOT EXISTS "${VCPKG_ROOT}")
    message("vcpkg submodule is not initialized, running git submodule update...")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE result)
    if(result)
        message(FATAL_ERROR "Git submodule update failed")
    endif()
endif()

if(WIN32)
    set(VCPKG_EXECUTABLE "${VCPKG_ROOT}/vcpkg.exe")
    set(BOOTSTRAP_SCRIPT bootstrap-vcpkg.bat)
else()
    set(VCPKG_EXECUTABLE "${VCPKG_ROOT}/vcpkg")
    set(BOOTSTRAP_SCRIPT bootstrap-vcpkg.sh)
endif()

if(NOT EXISTS ${VCPKG_EXECUTABLE})
    message("Vcpkg is not built, running bootstrap...")

    execute_process(COMMAND "${VCPKG_ROOT}/${BOOTSTRAP_SCRIPT}" RESULT_VARIABLE result)
    if(result)
        message(FATAL_ERROR "Bootstrap failed")
    endif()
endif()
 
# Set up and configure compilation
file(GLOB_RECURSE source_files src/*.cpp include/*.h include/*.hpp)

find_package(CommonLibSSE CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

set(PUBLIC_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}")
set(headers)

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
 
source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        ${headers}
        ${sources})


add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${headers} ${sources})
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")


target_precompile_headers(${PROJECT_NAME} PRIVATE include/PCH.h)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
        $<INSTALL_INTERFACE:src>)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)


install(TARGETS ${PROJECT_NAME}
        DESTINATION "${CMAKE_INSTALL_LIBDIR}")

message("Options:")
if(CMAKE_BUILD_TYPE MATCHES Debug)
    option(BUILD_TESTS "Build unit tests." ON)
else()
    option(BUILD_TESTS "Build unit tests." OFF)
endif()
message("\tTests: ${BUILD_TESTS}")

# handle tests
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2)
    enable_testing()
    add_subdirectory(tests)
endif()


add_subdirectory(tools)