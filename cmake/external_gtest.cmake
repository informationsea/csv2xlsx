include(ExternalProject)

# https://qiita.com/usagi/items/c5715c50bb56b65d0cd5
ExternalProject_Add(
    googletest
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/googletest
    INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/external/googletest-install
    GIT_REPOSITORY    https://github.com/google/googletest.git
    GIT_TAG           release-1.10.0
    CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/googletest-install
                      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

set(GOOGLETEST_LIBRARY gtest)
set(GOOGLETEST_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/external/googletest-install/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/external/googletest-install/lib ${CMAKE_CURRENT_BINARY_DIR}/external/googletest-install/lib64)
