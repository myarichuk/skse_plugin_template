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

# This will replace @PROJECT_NAME@ in template.json.in with the actual project name,
# and generate template.json in the source folder of your tool
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/template.json.in ${CMAKE_CURRENT_SOURCE_DIR}/EspGenerator/SynthesisMeta.json @ONLY)


message("Building the empty ESP generator")
# Now compile your .NET tool (make sure to adjust paths and names)
execute_process(COMMAND ${DOTNET} publish ${CMAKE_CURRENT_SOURCE_DIR}/EspGenerator/EspGenerator.csproj -o ${CMAKE_CURRENT_SOURCE_DIR}/ --self-contained true /p:PublishSingleFile=true -r win-x64
                               RESULT_VARIABLE result
                               OUTPUT_VARIABLE output
                               ERROR_VARIABLE error)
if(result)
    message(FATAL_ERROR \"Publishing .NET tool failed: ${error}\")
endif()