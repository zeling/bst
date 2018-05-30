cmake_minimum_required(VERSION 3.6)
project(skiplist)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(ExternalProject)
find_package(Git REQUIRED)

cmake_minimum_required(VERSION 2.8.8)
project(catch_builder CXX)
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
        catch
        PREFIX ${CMAKE_BINARY_DIR}/catch
        GIT_REPOSITORY https://github.com/philsquared/Catch.git
        TIMEOUT 10
        UPDATE_COMMAND ${GIT_EXECUTABLE} pull
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
)

# Expose required variable (CATCH_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(catch source_dir)
set(CATCH_INCLUDE_DIR ${source_dir}/single_include CACHE INTERNAL "Path to include folder for Catch")
#add_subdirectory(${EXT_PROJECTS_DIR}/catch)

set(SOURCE_FILES test.cc skiplist.h)
add_executable(test-bst ${SOURCE_FILES})
target_include_directories(test-bst PUBLIC ${CATCH_INCLUDE_DIR})
enable_testing(true)  # Enables unit-testing.
