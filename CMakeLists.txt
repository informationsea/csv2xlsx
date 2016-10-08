cmake_minimum_required(VERSION 3.0)
project(CSV2Excel)

ADD_LIBRARY(libxlsx  libxlsxwriter/src/app.c libxlsxwriter/src/chart.c 
	libxlsxwriter/src/content_types.c libxlsxwriter/src/core.c
	 libxlsxwriter/src/custom.c libxlsxwriter/src/drawing.c
	 libxlsxwriter/src/format.c libxlsxwriter/src/hash_table.c
	 libxlsxwriter/src/packager.c libxlsxwriter/src/relationships.c
	 libxlsxwriter/src/shared_strings.c libxlsxwriter/src/styles.c
	 libxlsxwriter/src/theme.c libxlsxwriter/src/utility.c
	 libxlsxwriter/src/workbook.c libxlsxwriter/src/worksheet.c
	 libxlsxwriter/src/xmlwriter.c)
TARGET_INCLUDE_DIRECTORIES(libxlsx PUBLIC libxlsxwriter/include)

add_executable(csv2excel main.cpp)

find_package(wxWidgets REQUIRED net gl core base)
include(${wxWidgets_USE_FILE})
# and for each of your dependent executable/library targets:
target_link_libraries(csv2excel ${wxWidgets_LIBRARIES})