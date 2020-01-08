include(ExternalProject)

# https://qiita.com/usagi/items/c5715c50bb56b65d0cd5
ExternalProject_Add(
    argtable3
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/argtable3
    INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/external/argtable3-install
    GIT_REPOSITORY    https://github.com/argtable/argtable3.git
    GIT_TAG           v3.1.5.1c1bb23
    CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/argtable3-install
                      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

set(ARGTABLE3_LIBRARY argtable3_static)
set(ARGTABLE3_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/external/argtable3-install/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/external/argtable3-install/lib)
