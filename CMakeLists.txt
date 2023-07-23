# CMakeList.txt : CMake project for skse_plugin_template, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.21)

set(PROJECT_NAME "skse_plugin_template")

# Set your project name. This will be the name of your SKSE .dll file.
project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX)
set(VCPKG_TARGET_TRIPLET x64-windows-static)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

file(GLOB_RECURSE source_files src/*.cpp include/*.h include/*.hpp)