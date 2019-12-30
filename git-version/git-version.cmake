cmake_minimum_required(VERSION 3.9)
message(STATUS "Resolving GIT Version")

set(build_version "unknown")

find_package(Git)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --always
        WORKING_DIRECTORY "${local_dir}"
        OUTPUT_VARIABLE build_version
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "Git version: ${build_version}")
else()
    message(WARNING "Git not found, version will be set to ${build_version}")
endif()

configure_file(
    ${local_dir}/git-version/gitversion.hpp.in 
    ${output_dir}/generated/gitversion.hpp @ONLY
)
