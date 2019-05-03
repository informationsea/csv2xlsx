add_subdirectory(googletest)

# zlib
add_subdirectory(zlib)

# xlsxwriter
file(GLOB LXW_SOURCES libxlsxwriter/src/*.c)
file(GLOB_RECURSE LXW_HEADERS RELATIVE libxlsxwriter/include *.h)
list(APPEND LXW_SOURCES libxlsxwriter/third_party/minizip/ioapi.c libxlsxwriter/third_party/minizip/zip.c)
if(MSVC)
    list(APPEND LXW_SOURCES libxlsxwriter/third_party/minizip/iowin32.c)
    set(CMAKE_C_FLAGS "/source-charset:utf-8")
endif()

list(APPEND LXW_SOURCES libxlsxwriter/third_party/tmpfileplus/tmpfileplus.c)

#set(LXW_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
#set(LXW_LIB_DIR "${LXW_PROJECT_DIR}/lib")
add_library(xlsxwriter "")
target_sources(xlsxwriter
    PRIVATE ${LXW_SOURCES}
    PUBLIC ${LXW_HEADERS}
)
target_include_directories(xlsxwriter PUBLIC libxlsxwriter/include zlib ${CMAKE_CURRENT_BINARY_DIR}/zlib)
target_link_libraries(xlsxwriter PRIVATE zlib)