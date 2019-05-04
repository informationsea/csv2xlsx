cmake_minimum_required(VERSION 3.4)
project(CSV2Xlsx)
enable_testing()

ADD_SUBDIRECTORY(thirdparty)
ADD_SUBDIRECTORY(libcsv)

ADD_LIBRARY(libcsv2xlsx src/libcsv2xlsx.c src/libcsv2xlsx.h)
TARGET_INCLUDE_DIRECTORIES(libcsv2xlsx PUBLIC libcsv thirdparty/libxlsxwriter/include)
TARGET_LINK_LIBRARIES(libcsv2xlsx PRIVATE csv xlsxwriter)

ADD_EXECUTABLE(csv2xlsx src/csv2xlsx.c)
TARGET_LINK_LIBRARIES(csv2xlsx PRIVATE libcsv2xlsx argtable3)

ADD_TEST(NAME csv2xlsx COMMAND csv2xlsx ${CMAKE_CURRENT_SOURCE_DIR}/libcsv/test/test1.csv -o test1.xlsx)
ADD_TEST(NAME csv2xlsx_2 COMMAND csv2xlsx ${CMAKE_CURRENT_SOURCE_DIR}/src/genes.csv -o genes.xlsx)

# add_executable(test-libcsv2xlsx src/test/test-libcsv2xlsx.cpp)
# add_test(NAME test-libcsv2xlsx COMMAND test-libcsv2xlsx)
# target_link_libraries(test-libcsv2xlsx PRIVATE gtest_main libcsv2xlsx)
# target_include_directories(test-libcsv2xlsx PRIVATE src thirdparty/libxlsxwriter/include)

#FILE(GLOB zlib_src ./zlib/*.c ./zlib/*.h)
#ADD_LIBRARY(zlib ${zlib_src})

#ADD_SUBDIRECTORY(csv2xlsx)
#ADD_SUBDIRECTORY(csv2xlsx-gui)
#ADD_SUBDIRECTORY(test)

install(FILES README.md LICENSE DESTINATION .)

#SET(CPACK_WIX_LICENSE_RTF "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_GENERATOR "ZIP")

INCLUDE(CPack)
