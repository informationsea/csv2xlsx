include(ExternalProject)

# https://qiita.com/usagi/items/c5715c50bb56b65d0cd5
ExternalProject_Add(
    libxlsxwriter
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter
    INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter-install
    GIT_REPOSITORY    https://github.com/jmcnamara/libxlsxwriter.git
    GIT_TAG           RELEASE_0.9.1
    CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter-install
                      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

set(LIBXLSXWRITER_LIBRARY xlsxwriter)
set(LIBXLSXWRITER_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter-install/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter-install/lib ${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter-install/lib64)
