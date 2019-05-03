cmake_minimum_required(VERSION 3.0)
project(CSV2Xlsx)
enable_testing()

ADD_SUBDIRECTORY(libcsv)
ADD_SUBDIRECTORY(googletest)
ADD_SUBDIRECTORY(libxlsxwriter)

ADD_LIBRARY(libcsv2xlsx csv2xlsx/libcsv2xlsx.c csv2xlsx/libcsv2xlsx.h)
TARGET_INCLUDE_DIRECTORIES(libcsv2xlsx PUBLIC libcsv libxlsxwriter/include)
TARGET_LINK_LIBRARIES(libcsv2xlsx PRIVATE libcsv xlsxwriter)

ADD_EXECUTABLE(csv2xlsx csv2xlsx/csv2xlsx.c)
TARGET_INCLUDE_DIRECTORIES(libcsv2xlsx PUBLIC libcsv libxlsxwriter/include)
TARGET_LINK_LIBRARIES(csv2xlsx PRIVATE libcsv2xlsx)

#FILE(GLOB zlib_src ./zlib/*.c ./zlib/*.h)
#ADD_LIBRARY(zlib ${zlib_src})

#ADD_SUBDIRECTORY(csv2xlsx)
#ADD_SUBDIRECTORY(csv2xlsx-gui)
#ADD_SUBDIRECTORY(test)

install(FILES README.md LICENSE DESTINATION .)

#SET(CPACK_WIX_LICENSE_RTF "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_GENERATOR "ZIP")

INCLUDE(CPack)
