cmake_minimum_required(VERSION 3.0)
project(CSV2Xlsx)
enable_testing()

ADD_SUBDIRECTORY(libcsv)
ADD_SUBDIRECTORY(googletest)

FILE(GLOB zlib_src ./zlib/*.c ./zlib/*.h)
ADD_LIBRARY(zlib ${zlib_src})

FILE(GLOB libxlsx_src ./libxlsxwriter/src/*.c libxlsxwriter/include/*.h)
ADD_LIBRARY(libxlsx 
	${libxlsx_src}
    libxlsxwriter/third_party/minizip/ioapi.c
    libxlsxwriter/third_party/minizip/iowin32.c
    libxlsxwriter/third_party/minizip/zip.c
    libxlsxwriter/third_party/minizip/unzip.c
    libxlsxwriter/third_party/minizip/mztools.c
    libxlsxwriter/third_party/tmpfileplus/tmpfileplus.c
	)
TARGET_INCLUDE_DIRECTORIES(libxlsx PUBLIC libxlsxwriter/include zlib)

ADD_SUBDIRECTORY(csv2xlsx)
ADD_SUBDIRECTORY(csv2xlsx-gui)
ADD_SUBDIRECTORY(test)

