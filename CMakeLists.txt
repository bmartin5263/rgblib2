cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS
        "src"
        "lib"
)

if(NOT DEFINED BUILD_TYPE)
    set(BUILD_TYPE "debug")
endif()

set(SDKCONFIG_DEFAULTS
        "sdkconfig.defaults"
        "sdkconfig.defaults.${BUILD_TYPE}"
)

project(rgblib)