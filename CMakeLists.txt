# CMakeList.txt : CMake project for skse_plugin_template, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.21)
message("Using toolchain file ${CMAKE_TOOLCHAIN_FILE}.")

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
 
#Set up and configure building of tools
# Locate the dotnet CLI tool
find_program(DOTNET dotnet)
if(NOT DOTNET)
    message(FATAL_ERROR "The dotnet CLI tool is not found. Please install it to proceed.")
endif()

# Check the version of .NET
execute_process(COMMAND ${DOTNET} --version
                OUTPUT_VARIABLE DOTNET_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)

# parse the version into major, minor and patch
string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)" DOTNET_VERSION ${DOTNET_VERSION})

set(DOTNET_VERSION_MAJOR ${CMAKE_MATCH_1})
set(DOTNET_VERSION_MINOR ${CMAKE_MATCH_2})
set(DOTNET_VERSION_PATCH ${CMAKE_MATCH_3})

# check if the version is at least 7.0.0
if(DOTNET_VERSION_MAJOR LESS 7)
    message(FATAL_ERROR "The dotnet version must be at least 7.0.0")
endif()

message("Building the empty ESP generator")
# Now compile your .NET tool (make sure to adjust paths and names)
# In this case, the .NET tool is in the same repo and can be built using 'dotnet build'
execute_process(COMMAND ${DOTNET} build ${CMAKE_CURRENT_SOURCE_DIR}/tools/EspGenerator/EspGenerator.csproj
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output
                ERROR_VARIABLE error)
  
if(result)
    message(FATAL_ERROR "Building .NET tool failed: ${error}")
endif()

# Add the .NET tool to the install phase to generate the empty ESP
# We publish the .NET tool to /tools with everything required to run it standalone
install(CODE "execute_process(COMMAND ${DOTNET} publish ${CMAKE_CURRENT_SOURCE_DIR}/tools/EspGenerator/EspGenerator.csproj -o ${CMAKE_CURRENT_SOURCE_DIR}/tools --self-contained true -r win-x64
                               RESULT_VARIABLE result
                               OUTPUT_VARIABLE output
                               ERROR_VARIABLE error)
             if(result)
                 message(FATAL_ERROR \"Publishing .NET tool failed: ${error}\")
             endif()")


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