# Google test
include(ExternalProject)
ExternalProject_Add(
    googletest
    URL https://github.com/google/googletest/archive/release-1.8.1.zip
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/googletest-install -Dgtest_force_shared_crt=true
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/googletest-install
)
add_library(gtest STATIC IMPORTED GLOBAL)
set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX})
set_target_properties(gtest PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/include)
add_dependencies(gtest googletest)

add_library(gtest_main STATIC IMPORTED GLOBAL)
set_target_properties(gtest_main PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX})
set_target_properties(gtest_main PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/include)
set_target_properties(gtest_main PROPERTIES IMPORTED_LINK_INTERFACE_LIBRARIES gtest)
add_dependencies(gtest_main googletest)

# Zlib
ExternalProject_Add(
    zlib
    URL https://github.com/madler/zlib/archive/v1.2.11.zip
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/zlib-install 
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/zlib-install
)
add_library(zlibstatic STATIC IMPORTED GLOBAL)
set_target_properties(zlibstatic PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/zlib-install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX})
set_target_properties(zlibstatic PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/zlib-install/include)
add_dependencies(zlibstatic zlib)

# Argtable3
ExternalProject_Add(
    argtable3_upstream
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/argtable3-prefix
    URL https://github.com/argtable/argtable3/archive/v3.1.2.bb37058.tar.gz
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/argtable3-install 
    BUILD_COMMAND ${CMAKE_COMMAND} --build . --target argtable3_static
    INSTALL_COMMAND ""
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/argtable3-install
)

add_library(argtable3 STATIC IMPORTED GLOBAL)
set_target_properties(argtable3 PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/argtable3-prefix/src/argtable3_upstream-build/src/${CMAKE_STATIC_LIBRARY_PREFIX}argtable3_static${CMAKE_STATIC_LIBRARY_SUFFIX})
set_target_properties(argtable3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/argtable3-prefix/src/argtable3_upstream/src)
add_dependencies(argtable3 argtable3_upstream)

# xlsxwriter
ExternalProject_Add(
    xlsxwriter_upstream
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/xlsxwriter-prefix
    URL https://github.com/jmcnamara/libxlsxwriter/archive/RELEASE_0.8.6.tar.gz
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/xlsxwriter-install -DZLIB_INLUCD_DIR=${CMAKE_CURRENT_BINARY_DIR}/zlib-install/include -DZLIB_LIBRARY=${CMAKE_CURRENT_BINARY_DIR}/zlib-install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/xlsxwriter-install
)
add_library(xlsxwriter STATIC IMPORTED GLOBAL)
set_target_properties(xlsxwriter PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/xlsxwriter-install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}xlsxwriter${CMAKE_STATIC_LIBRARY_SUFFIX})
set_target_properties(xlsxwriter PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/xlsxwriter-install/include)
set_target_properties(xlsxwriter PROPERTIES IMPORTED_LINK_INTERFACE_LIBRARIES zlibstatic)
add_dependencies(xlsxwriter_upstream zlib)
add_dependencies(xlsxwriter xlsxwriter_upstream)
