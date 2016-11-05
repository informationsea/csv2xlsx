add_executable(csv2xlsx-gui main.cpp ConvertThread.cpp ConvertThread.hpp)

find_package(wxWidgets REQUIRED net gl core base)
include(${wxWidgets_USE_FILE})
# and for each of your dependent executable/library targets:
target_link_libraries(csv2xlsx-gui ${wxWidgets_LIBRARIES} libxlsx)

TARGET_INCLUDE_DIRECTORIES(csv2xlsx-gui PUBLIC ../csv2xlsx ../libcsv ../libxlsxwriter/include)
TARGET_LINK_LIBRARIES(csv2xlsx-gui libcsv2xlsx libcsv libxlsx zlib)


if(WIN32)
   set_target_properties(csv2xlsx-gui PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
   set_target_properties(csv2xlsx-gui PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
   set_target_properties(csv2xlsx-gui PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
   set_target_properties(csv2xlsx-gui PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
   set_target_properties(csv2xlsx-gui PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
   set_target_properties(csv2xlsx-gui PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif(WIN32)
