ADD_LIBRARY(libcsv2xlsx libcsv2xlsx.c libcsv2xlsx.h)
TARGET_INCLUDE_DIRECTORIES(libcsv2xlsx PUBLIC ../libcsv ../libxlsxwriter/include)
TARGET_LINK_LIBRARIES(libcsv2xlsx libcsv libxlsx zlib)

ADD_EXECUTABLE(csv2xlsx csv2xlsx.c)
TARGET_INCLUDE_DIRECTORIES(libcsv2xlsx PUBLIC ../libcsv ../libxlsxwriter/include)
TARGET_LINK_LIBRARIES(csv2xlsx libcsv libxlsx libcsv2xlsx zlib)

ADD_TEST(NAME csv2xlsx COMMAND csv2xlsx ${CSV2Excel_SOURCE_DIR}/test/test1.csv test1.xlsx)

