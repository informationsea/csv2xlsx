include(ExternalProject)

# https://qiita.com/usagi/items/c5715c50bb56b65d0cd5
ExternalProject_Add(
    argtable3
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/argtable3
    INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/external/argtable3-install
    GIT_REPOSITORY    https://github.com/argtable/argtable3.git
    GIT_TAG           v3.2.2.f25c624
    CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/argtable3-install
                      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ARGTABLE3_LIBRARY argtable3_staticd)
else()
    set(ARGTABLE3_LIBRARY argtable3_static)
endif()
set(ARGTABLE3_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/external/argtable3-install/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/external/argtable3-install/lib ${CMAKE_CURRENT_BINARY_DIR}/external/argtable3-install/lib64)
