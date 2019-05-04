add_library(csv libcsv.h libcsv.c)
target_include_directories(csv PUBLIC .)

ADD_EXECUTABLE(test_libcsv test/test_libcsv.cpp test/test1.csv)
TARGET_LINK_LIBRARIES(test_libcsv csv gtest_main gtest)

ADD_EXECUTABLE(test_libcsv2 test/test_libcsv2.cpp test/simple1.csv)
TARGET_LINK_LIBRARIES(test_libcsv2 csv gtest_main gtest)

ADD_TEST(NAME test_libcsv1 COMMAND test_libcsv)
SET_TESTS_PROPERTIES(test_libcsv1 PROPERTIES ENVIRONMENT CSV1=${CMAKE_CURRENT_SOURCE_DIR}/test/test1.csv)

ADD_TEST(NAME test_libcsv2 COMMAND test_libcsv2)
SET_TESTS_PROPERTIES(test_libcsv2 PROPERTIES ENVIRONMENT CSV1=${CMAKE_CURRENT_SOURCE_DIR}/test/simple1.csv)

ADD_TEST(NAME test_libcsv2_2 COMMAND test_libcsv2)
SET_TESTS_PROPERTIES(test_libcsv2_2 PROPERTIES ENVIRONMENT CSV1=${CMAKE_CURRENT_SOURCE_DIR}/test/simple2.csv)

ADD_TEST(NAME test_libcsv2_3 COMMAND test_libcsv2)
SET_TESTS_PROPERTIES(test_libcsv2_3 PROPERTIES ENVIRONMENT CSV1=${CMAKE_CURRENT_SOURCE_DIR}/test/simple3.csv)