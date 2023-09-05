if(WIN32)

include(ExternalProject)

# https://qiita.com/usagi/items/c5715c50bb56b65d0cd5
ExternalProject_Add(
    zlib
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/zlib
    INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/external/zlib-install
    GIT_REPOSITORY    https://github.com/madler/zlib.git
    GIT_TAG           v1.3
    CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/zlib-install
                      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

    message("build type " ${CMAKE_BUILD_TYPE})
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(ZLIB_LIBRARIES zlibstaticd)
    else()
        set(ZLIB_LIBRARIES zlibstatic)
    endif()
set(ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/external/zlib-install/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/external/zlib-install/lib)

else()

find_package(ZLIB REQUIRED)

endif()
