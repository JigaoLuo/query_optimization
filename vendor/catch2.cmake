find_package(Git REQUIRED)
include(ExternalProject)
ExternalProject_Add(
    catch_src
    PREFIX ${CMAKE_BINARY_DIR}/catch
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.3
    TIMEOUT 20
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
   )
ExternalProject_Get_Property(catch_src source_dir)
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE ${source_dir}/single_include)