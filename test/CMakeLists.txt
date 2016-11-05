ADD_EXECUTABLE(test_libcsv test_libcsv.cpp test1.csv)
TARGET_INCLUDE_DIRECTORIES(test_libcsv PUBLIC ../libcsv ../googletest)
TARGET_LINK_LIBRARIES(test_libcsv libcsv gtest)

ADD_EXECUTABLE(test_libcsv2 test_libcsv2.cpp simple1.csv)
TARGET_INCLUDE_DIRECTORIES(test_libcsv2 PUBLIC ../libcsv ../googletest)
TARGET_LINK_LIBRARIES(test_libcsv2 libcsv gtest)

ADD_TEST(NAME test_libcsv1 COMMAND test_libcsv)
SET_TESTS_PROPERTIES(test_libcsv1 PROPERTIES ENVIRONMENT CSV1=${CSV2Xlsx_SOURCE_DIR}/test/test1.csv)

ADD_TEST(NAME test_libcsv2 COMMAND test_libcsv2)
SET_TESTS_PROPERTIES(test_libcsv2 PROPERTIES ENVIRONMENT CSV1=${CSV2Xlsx_SOURCE_DIR}/test/simple1.csv)

ADD_TEST(NAME test_libcsv2_2 COMMAND test_libcsv2)
SET_TESTS_PROPERTIES(test_libcsv2_2 PROPERTIES ENVIRONMENT CSV1=${CSV2Xlsx_SOURCE_DIR}/test/simple2.csv)

ADD_TEST(NAME test_libcsv2_3 COMMAND test_libcsv2)
SET_TESTS_PROPERTIES(test_libcsv2_3 PROPERTIES ENVIRONMENT CSV1=${CSV2Xlsx_SOURCE_DIR}/test/simple3.csv)
