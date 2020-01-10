cmake_minimum_required(VERSION 3.4)
project(csv2xlsx VERSION 0.2.2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

find_package(Threads REQUIRED)
include(cmake/external_zlib.cmake)
include(cmake/external_libxlsxwriter.cmake)
include(cmake/external_argtable3.cmake)

enable_testing()

ADD_SUBDIRECTORY(libcsv)

ADD_LIBRARY(libcsv2xlsx src/libcsv2xlsx.c src/libcsv2xlsx.h)
add_dependencies(libcsv2xlsx libxlsxwriter argtable3)
TARGET_INCLUDE_DIRECTORIES(libcsv2xlsx PUBLIC libcsv ${LIBXLSXWRITER_INCLUDE_DIRS} ${ARGTABLE3_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
#print("libxlsx" ${LIBXLSXWRITER_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(libcsv2xlsx PRIVATE csv ${LIBXLSXWRITER_LIBRARY} ${ZLIB_LIBRARIES})

ADD_EXECUTABLE(csv2xlsx src/csv2xlsx.c)
TARGET_LINK_LIBRARIES(csv2xlsx PUBLIC libcsv2xlsx ${ARGTABLE3_LIBRARY} ${ARGTABLE3_LIBRARY})
install(TARGETS csv2xlsx RUNTIME DESTINATION bin)

ADD_TEST(NAME csv2xlsx COMMAND csv2xlsx ${CMAKE_CURRENT_SOURCE_DIR}/libcsv/test/test1.csv -o test1.xlsx)
ADD_TEST(NAME csv2xlsx_2 COMMAND csv2xlsx ${CMAKE_CURRENT_SOURCE_DIR}/src/genes.csv -o genes.xlsx)

install(FILES README.md LICENSE DESTINATION .)
SET(CPACK_GENERATOR "ZIP")
SET(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
SET(CPACK_PACKAGE_CHECKSUM "SHA256")
INCLUDE(CPack)
