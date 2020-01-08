include(../cmake/external_gtest.cmake)

add_library(csv libcsv.h libcsv.c)
add_dependencies(csv googletest)
target_include_directories(csv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${GOOGLETEST_INCLUDE_DIRS})

ADD_EXECUTABLE(test_libcsv test/test_libcsv.cpp test/test1.csv)
TARGET_LINK_LIBRARIES(test_libcsv csv gtest_main ${GOOGLETEST_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

ADD_EXECUTABLE(test_libcsv2 test/test_libcsv2.cpp test/simple1.csv)
TARGET_LINK_LIBRARIES(test_libcsv2 csv gtest_main ${GOOGLETEST_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

ADD_TEST(NAME test_libcsv1 COMMAND test_libcsv)
SET_TESTS_PROPERTIES(test_libcsv1 PROPERTIES ENVIRONMENT CSV1=${CMAKE_CURRENT_SOURCE_DIR}/test/test1.csv)

ADD_TEST(NAME test_libcsv2 COMMAND test_libcsv2)
SET_TESTS_PROPERTIES(test_libcsv2 PROPERTIES ENVIRONMENT CSV1=${CMAKE_CURRENT_SOURCE_DIR}/test/simple1.csv)

ADD_TEST(NAME test_libcsv2_2 COMMAND test_libcsv2)
SET_TESTS_PROPERTIES(test_libcsv2_2 PROPERTIES ENVIRONMENT CSV1=${CMAKE_CURRENT_SOURCE_DIR}/test/simple2.csv)

ADD_TEST(NAME test_libcsv2_3 COMMAND test_libcsv2)
SET_TESTS_PROPERTIES(test_libcsv2_3 PROPERTIES ENVIRONMENT CSV1=${CMAKE_CURRENT_SOURCE_DIR}/test/simple3.csv)
