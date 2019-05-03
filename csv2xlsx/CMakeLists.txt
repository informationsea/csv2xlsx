ADD_LIBRARY(libcsv2xlsx libcsv2xlsx.c libcsv2xlsx.h)
TARGET_INCLUDE_DIRECTORIES(libcsv2xlsx PUBLIC ../libcsv ../libxlsxwriter/include)
TARGET_LINK_LIBRARIES(libcsv2xlsx PRIVATE libcsv libxlsxwriter)

ADD_EXECUTABLE(csv2xlsx csv2xlsx.c)
TARGET_INCLUDE_DIRECTORIES(libcsv2xlsx PUBLIC ../libcsv ../libxlsxwriter/include)
TARGET_LINK_LIBRARIES(csv2xlsx PRIVATE libcsv2xlsx)

ADD_TEST(NAME csv2xlsx COMMAND csv2xlsx ${CSV2Xlsx_SOURCE_DIR}/test/test1.csv test1.xlsx)
ADD_TEST(NAME csv2xlsx_2 COMMAND csv2xlsx ${CSV2Xlsx_SOURCE_DIR}/csv2xlsx/genes.csv genes.xlsx)

install(TARGETS csv2xlsx RUNTIME DESTINATION bin)
