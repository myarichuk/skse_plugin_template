# CMakeList.txt : CMake project for skse_plugin_template, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.21)

set(PROJECT_NAME "skse_plugin_template")

# Set your project name. This will be the name of your SKSE .dll file.
project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Set up and configure VCPKG
set(VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")

if(NOT EXISTS "${VCPKG_ROOT}/installed")
    message("Vcpkg is not installed or built, running bootstrap...")
    execute_process(COMMAND "${VCPKG_ROOT}/bootstrap-vcpkg.sh" RESULT_VARIABLE result)
    if(result)
        message(FATAL_ERROR "Bootstrap failed")
    endif()
endif()

file(GLOB_RECURSE source_files src/*.cpp include/*.h include/*.hpp)