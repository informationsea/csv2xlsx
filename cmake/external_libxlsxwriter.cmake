include(ExternalProject)

# https://qiita.com/usagi/items/c5715c50bb56b65d0cd5
ExternalProject_Add(
    libxlsxwriter
    DEPENDS           zlib
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter
    INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter-install
    GIT_REPOSITORY    https://github.com/jmcnamara/libxlsxwriter.git
    GIT_TAG           f477741dd3782101eefb35f9c6f9ed93ee3f642d
    CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter-install
                      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                      -DZLIB_LIBRARY=${CMAKE_CURRENT_BINARY_DIR}/external/zlib-install/lib/${ZLIB_LIBRARIES}.lib
                      -DZLIB_INCLUDE_DIR=${CMAKE_CURRENT_BINARY_DIR}/external/zlib-install/include
)

set(LIBXLSXWRITER_LIBRARY xlsxwriter)
set(LIBXLSXWRITER_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter-install/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter-install/lib ${CMAKE_CURRENT_BINARY_DIR}/external/libxlsxwriter-install/lib64)
